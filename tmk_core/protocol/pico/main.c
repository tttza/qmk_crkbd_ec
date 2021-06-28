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

#include "pico/stdio/driver.h"
#include "bsp/board.h"
#include "tusb.h"
#include "qmk_main.h"

//--------------------------------------------------------------------+
// MACRO CONSTANT TYPEDEF PROTYPES
//--------------------------------------------------------------------+
static int cdc_in_chars(char* buf, int len) { return 0; }

static stdio_driver_t stdio_driver = {.out_chars = tud_cdc_write,
                                      .out_flush = tud_cdc_write_flush,
                                      .in_chars  = cdc_in_chars,
                                      .next      = NULL};

/*------------- MAIN -------------*/
int main(void) {
    board_init();
    tusb_init();

    stdio_set_driver_enabled(&stdio_driver, true);

    qmk_init();

    while (1) {
        tud_task();  // tinyusb device task

        qmk_task();
    }

    return 0;
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

// Invoked when cdc when line state changed e.g connected/disconnected
void tud_cdc_line_state_cb(uint8_t itf, bool dtr, bool rts) {
    (void)itf;
    (void)rts;

    // TODO set some indicator
    if (dtr) {
        // Terminal connected
    } else {
        // Terminal disconnected
    }
}

// Invoked when CDC interface received data from host
void tud_cdc_rx_cb(uint8_t itf) { (void)itf; }

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

// Invoked when received SET_REPORT control request or
// received data on OUT endpoint ( Report ID = 0, Type = 0 )
void tud_hid_set_report_cb(uint8_t itf, uint8_t report_id,
                           hid_report_type_t report_type, uint8_t const* buffer,
                           uint16_t bufsize) {
    // TODO set LED based on CAPLOCK, NUMLOCK etc...
    (void)itf;
    (void)report_id;
    (void)report_type;
    (void)buffer;
    (void)bufsize;
}
