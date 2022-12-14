/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2019 Ha Thach (tinyusb.org)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "qmk_main.h"
#include "raw_hid.h"
#include "bootloader.h"
#include "debug.h"

#include "eeprom_pico.h"
#include "usb_descriptors.h"

#include "pico/stdio/driver.h"
#include "hardware/watchdog.h"
#include "tusb.h"

void platform_setup(void);
extern char __StackTop;

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
static int cdc_in_chars(char* buf, int len) { return 0; }

static void cdc_write(const char* buf, int length) {
    static uint64_t last_avail_time;
    if (tud_cdc_connected()) {
        for (int i = 0; i < length;) {
            int n     = length - i;
            int avail = tud_cdc_write_available();
            if (n > avail) n = avail;
            if (n) {
                int n2 = tud_cdc_write(buf + i, n);
                tud_task();
                tud_cdc_write_flush();
                i += n2;
                last_avail_time = time_us_64();
            } else {
                tud_task();
                tud_cdc_write_flush();
                if (!tud_cdc_connected() ||
                    (!tud_cdc_write_available() &&
                     time_us_64() > last_avail_time + 500000)) {
                    break;
                }
            }
        }
    }
}

static stdio_driver_t stdio_driver = {.out_chars = cdc_write,
                                      .out_flush = NULL,
                                      .in_chars  = cdc_in_chars,
                                      .next      = NULL};

void pico_cdc_enable_printf(void) {
    stdio_set_driver_enabled(&stdio_driver, true);
}
void pico_cdc_disable_printf(void) {
    stdio_set_driver_enabled(&stdio_driver, false);
}

void protocol_setup(void) {
    tusb_init();
    pico_cdc_enable_printf();
    EEPROM_Init();
}

void protocol_init(void) { qmk_init(); }

void protocol_task(void) {
    tud_task();
    qmk_task();
    watchdog_update();
}

//--------------------------------------------------------------------+
// Device callbacks
//--------------------------------------------------------------------+

// Invoked when device is mounted
void tud_mount_cb(void) {}

// Invoked when device is unmounted
void tud_umount_cb(void) {}

// Invoked when usb bus is suspended
// remote_wakeup_en : if host allow us  to perform remote wakeup
// Within 7ms, device must draw an average of current less than 2.5 mA from bus
void tud_suspend_cb(bool remote_wakeup_en) { (void)remote_wakeup_en; }

// Invoked when usb bus is resumed
void tud_resume_cb(void) {}

__attribute__((weak)) void pico_cdc_on_connect(void) {}
__attribute__((weak)) void pico_cdc_on_disconnect(void) {}

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    (void)itf;
    (void)rts;

    // TODO set some indicator
    if (dtr) {
        pico_cdc_on_connect();
    } else {
        pico_cdc_on_disconnect();
    }
}

__attribute__((weak)) void pico_cdc_change_baudrate_cb(uint32_t baudrate) {
    if (baudrate == 1200) {
        bootloader_jump();
    }
}

void tud_cdc_line_coding_cb(uint8_t                  itf,
                            cdc_line_coding_t const* p_line_coding) {
    (void)itf;
    pico_cdc_change_baudrate_cb(p_line_coding->bit_rate);
}

__attribute__((weak)) bool pico_cdc_receive_kb(uint8_t const* buf,
                                               uint32_t       cnt) {
    return true;
}

__attribute__((weak)) void pico_cdc_receive_cb(uint8_t const* buf,
                                               uint32_t       cnt) {
    tud_cdc_write(buf, cnt);
    tud_cdc_write_flush();

    if (!pico_cdc_receive_kb(buf, cnt)) {
        return;
    }

    if (buf[0] == 'b') {
        bootloader_jump();
    } else if (buf[0] == 'l') {
        printf("init eeprom emulation\n");
        EEPROM_Init();
        printf("complete\n");
    } else if (buf[0] == 'c') {
        printf("clear eeprom emulation\n");
        EEPROM_Erase();
        printf("complete\n");
    } else if (buf[0] == 'd') {
        printf("debug print ");
        if (!debug_enable) {
            debug_enable = true;
            dprint("true\n");
        } else {
            debug_enable = false;
            print("false\n");
        }
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) {
    if (tud_cdc_available()) {
        // read datas
        char     buf[64];
        uint32_t count = tud_cdc_read(buf, sizeof(buf));
        (void)count;

        pico_cdc_receive_cb((uint8_t*)buf, count);
    }
}

// Invoked when sent REPORT successfully to host
// Application can use this to send the next report
// Note: For composite reports, report[0] is report ID
void tud_hid_report_complete_cb(uint8_t itf, uint8_t const* report,
                                uint8_t len) {
    (void)itf;
    (void)len;
}

// Invoked when received GET_REPORT control request
// Application must fill buffer report's content and return its length.
// Return zero will cause the stack to STALL request
uint16_t tud_hid_get_report_cb(uint8_t itf, uint8_t report_id,
                               hid_report_type_t report_type, uint8_t* buffer,
                               uint16_t reqlen) {
    // TODO not Implemented
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)reqlen;

    return 0;
}

static uint8_t keyboard_indicator_led;
uint8_t        get_keyboard_led_status(void) { return keyboard_indicator_led; }

__attribute__((weak)) void raw_hid_receive(uint8_t* data, uint8_t length) {}

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const* buffer,
                           uint16_t bufsize) {
    switch (itf) {
        case ITF_NUM_HID_KEYBOARD:
            // LED status
            keyboard_indicator_led = buffer[0];
            break;
        case ITF_NUM_HID_RAW:
            raw_hid_receive((uint8_t*)buffer, bufsize);
            break;
        default:
            break;
    }
}
