/*
 * (c) 2015 flabberast <s3+flabbergast@sdfeu.org>
 *
 * Based on the following work:
 *  - Guillaume Duc's raw hid example (MIT License)
 *    https://github.com/guiduc/usb-hid-chibios-example
 *  - PJRC Teensy examples (MIT License)
 *    https://www.pjrc.com/teensy/usb_keyboard.html
 *  - hasu's TMK keyboard code (GPL v2 and some code Modified BSD)
 *    https://github.com/tmk/tmk_keyboard/
 *  - ChibiOS demo code (Apache 2.0 License)
 *    http://www.chibios.org
 *
 * Since some GPL'd code is used, this work is licensed under
 * GPL v2 or later.
 */

#include <stdbool.h>
#include <string.h>

/* TMK includes */
#include "report.h"
#include "host.h"
#include "host_driver.h"
#include "keyboard.h"
#include "action.h"
#include "action_util.h"
#include "mousekey.h"
#include "led.h"
#include "sendchar.h"
#include "debug.h"
#include "print.h"

#ifdef SLEEP_LED_ENABLE
#    include "sleep_led.h"
#endif
#ifdef SERIAL_LINK_ENABLE
#    include "serial_link/system/serial_link.h"
#endif
#ifdef VISUALIZER_ENABLE
#    include "visualizer/visualizer.h"
#endif
#ifdef MIDI_ENABLE
#    include "qmk_midi.h"
#endif
#ifdef STM32_EEPROM_ENABLE
#    include "eeprom_stm32.h"
#endif
#ifdef EEPROM_DRIVER
#    include "eeprom_driver.h"
#endif
#include "suspend.h"
#include "wait.h"

#include "iusb.h"
#include "tusb.h"
#include "usb_descriptors.h"

/* host struct */
host_driver_t driver = {
    .keyboard_leds = keyboard_leds,
    .send_keyboard = send_keyboard,
    .send_nkro     = NULL,
    .send_mouse    = send_mouse,
    .send_extra    = send_extra,
#ifdef RAW_ENABLE
    .send_raw_hid = pico_raw_hid_send,
#endif
};

#ifdef CONSOLE_ENABLE
static uint8_t console_buffer[CONSOLE_EPSIZE];
static uint8_t console_count = 0;

static bool pico_console_flush(void) {
    if (console_count == 0) {
        return true;
    }

    tud_task();
    if (!tud_ready()) {
        return false;
    }

    if (!tud_hid_n_ready(ITF_NUM_HID_CONSOLE)) {
        tud_task();
        if (!tud_hid_n_ready(ITF_NUM_HID_CONSOLE)) {
            return false;
        }
    }

    uint8_t report[CONSOLE_EPSIZE] = {0};
    memcpy(report, console_buffer, console_count);
    tud_hid_n_report(ITF_NUM_HID_CONSOLE, 0, report, sizeof(report));
    tud_task();
    console_count = 0;
    return true;
}

int8_t sendchar(uint8_t c) {
    if (console_count >= sizeof(console_buffer)) {
        if (!pico_console_flush()) {
            return -1;
        }
    }

    if (console_count < sizeof(console_buffer)) {
        console_buffer[console_count++] = c;
    }

    pico_console_flush();
    return 0;
}

void console_task(void) {
    pico_console_flush();
}
#endif

__attribute__((weak)) void raw_hid_task(void) {
    // TODO
}

static void midi_ep_task(void) {
}

void qmk_init(void) {
    keyboard_setup();

#ifdef MIDI_ENABLE
    setup_midi();
#endif

    keyboard_init();
    host_set_driver(&driver);
}

void qmk_task(void) {
    keyboard_task();
#ifdef CONSOLE_ENABLE
    console_task();
#endif
#ifdef MIDI_ENABLE
    midi_ep_task();
#endif
#ifdef VIRTSER_ENABLE
    virtser_task();
#endif
#ifdef RAW_ENABLE
    raw_hid_task();
#endif
}
