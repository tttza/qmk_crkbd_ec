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

/* host struct */
host_driver_t driver = {keyboard_leds, send_keyboard, send_mouse, send_system,
                        send_consumer};

static void console_task(void) {
    // TODO
}

static void raw_hid_task(void) {
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
