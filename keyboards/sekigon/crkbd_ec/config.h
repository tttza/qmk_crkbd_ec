/*
Copyright 2022 sekigon-gonnoc

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include "config_common.h"

#if !defined(__ASSEMBLER__) && !defined(__cplusplus)
#    include "quantum/nvm/eeprom/nvm_eeprom_eeconfig_internal.h"
#endif

// Limit dynamic keymap usage so it fits inside the RP2040 EEPROM window.
#ifdef DYNAMIC_KEYMAP_EEPROM_MAX_ADDR
#    undef DYNAMIC_KEYMAP_EEPROM_MAX_ADDR
#endif
#define DYNAMIC_KEYMAP_EEPROM_MAX_ADDR 2047

// Prevent accidental double-reset from dropping into BOOTSEL while debugging.
#define PICO_BOOTSEL_VIA_DOUBLE_RESET 0
// Disable platform watchdog to avoid falling back to BOOTSEL on Xiao RP2040.
#define PICO_WATCHDOG_TIMEOUT_MS 0

/* key matrix size */
#define MATRIX_ROWS 6
#define MATRIX_COLS 7

/* EC switch threshold with hysteresis */
#define HIGH_THRESHOLD 1200
#define LOW_THRESHOLD 600

// VIA config
#ifdef DYNAMIC_KEYMAP_LAYER_COUNT
#    undef DYNAMIC_KEYMAP_LAYER_COUNT
#endif
#define DYNAMIC_KEYMAP_LAYER_COUNT 8
#define VIA_CUSTOM_LIGHTING_ENABLE
#define EEPROM_ECS_THRESHOLD_ADDR (EECONFIG_SIZE)
#define VIA_RGBLIGHT_USER_ADDR (EEPROM_ECS_THRESHOLD_ADDR + 2 * 2)
#define VIA_EEPROM_MAGIC_ADDR \
    (VIA_RGBLIGHT_USER_ADDR + \
     DYNAMIC_KEYMAP_LAYER_COUNT * 4)  // Layer * 4bytes(RGB Light config)

/*
 * Keyboard Matrix Assignments
 *
 * Change this to how you wired your keyboard
 * COLS: AVR pins used for columns, left to right
 * ROWS: AVR pins used for rows, top to bottom
 * DIODE_DIRECTION: COL2ROW = COL = Anode (+), ROW = Cathode (-, marked on
 * diode) ROW2COL = ROW = Anode (+), COL = Cathode (-, marked on diode)
 *
 */
#define MATRIX_ROW_PINS {26, 27, 28}
#define MATRIX_COL_CHANNELS {0, 7, 1, 6, 2, 4, 3}
#define DISCHARGE_PIN 0
#define ANALOG_PORT 29
#define MUX_SEL_PINS {3, 4, 2}

/* COL2ROW, ROW2COL */
#define DIODE_DIRECTION COL2ROW

#define SOFT_SERIAL_PIN GP1
#define EE_HANDS

#define WS2812_DI_PIN GP7
#define RGBLED_NUM 42

#ifdef RGBLIGHT_ENABLE
#    define RGBLED_SPLIT {21, 21}
#    define RGBLIGHT_SPLIT
#    define RGBLIGHT_HUE_STEP 8
#    define RGBLIGHT_SAT_STEP 8
#    define RGBLIGHT_VAL_STEP 8
#    define RGBLIGHT_LIMIT_VAL 255 /* The maximum brightness level */
#    define RGBLIGHT_SLEEP /* If defined, the RGB lighting will be switched \
                              off when the host goes to sleep */
/*== all animations enable ==*/
#    define RGBLIGHT_ANIMATIONS
/*== customize breathing effect ==*/
/*==== (DEFAULT) use fixed table instead of exp() and sin() ====*/
#    define RGBLIGHT_BREATHE_TABLE_SIZE 256  // 256(default) or 128 or 64
/*==== use exp() and sin() ====*/
#    define RGBLIGHT_EFFECT_BREATHE_CENTER 1.85  // 1 to 2.7
#    define RGBLIGHT_EFFECT_BREATHE_MAX 255      // 0 to 255
#endif

#ifdef RGB_MATRIX_ENABLE
#    define DRIVER_LED_TOTAL RGBLED_NUM
#    define RGB_MATRIX_SPLIT {21, 21}
#    define SPLIT_TRANSPORT_MIRROR
#    define RGB_MATRIX_KEYPRESSES  // reacts to keypresses
// #   define RGB_MATRIX_KEYRELEASES // reacts to keyreleases (instead of
// keypresses) #   define RGB_DISABLE_AFTER_TIMEOUT 0 // number of ticks to wait
// until disabling effects
#    define RGB_DISABLE_WHEN_USB_SUSPENDED  // turn off effects when suspended
#    define RGB_MATRIX_FRAMEBUFFER_EFFECTS
// #   define RGB_MATRIX_LED_PROCESS_LIMIT (DRIVER_LED_TOTAL + 4) / 5 // limits
// the number of LEDs to process in an animation per task run (increases
// keyboard responsiveness) #   define RGB_MATRIX_LED_FLUSH_LIMIT 16 // limits
// in milliseconds how frequently an animation will update the LEDs. 16 (16ms)
// is equivalent to limiting to 60fps (increases keyboard responsiveness)
#    define RGB_MATRIX_MAXIMUM_BRIGHTNESS 150
// limits maximum brightness of LEDs to 150 out of 255. Higher may
// cause the controller to crash.
#    define RGB_MATRIX_HUE_STEP 8
#    define RGB_MATRIX_SAT_STEP 8
#    define RGB_MATRIX_VAL_STEP 8
#    define RGB_MATRIX_SPD_STEP 10
#endif

// #define WS2812_TIMING 1300
// #define WS2812_T0H 320
// #define WS2812_T1H 640
// #define WS2812_TRST_US 200
// #define WS2812_BYTE_ORDER WS2812_BYTE_ORDER_RGB

#define SPLIT_USB_TIMEOUT 3000
#define SPLIT_USB_TIMEOUT_POLL 25
// Temporarily disable split watchdog while debugging boot issues.
// #define SPLIT_WATCHDOG_ENABLE
// #define SPLIT_WATCHDOG_TIMEOUT 6000

/* Debounce reduces chatter (unintended double-presses) - set 0 if debouncing is
 * not needed */
#define DEBOUNCE 5

/* define if matrix has ghost (lacks anti-ghosting diodes) */
// #define MATRIX_HAS_GHOST

#define PERMISSIVE_HOLD

/* Mechanical locking support. Use KC_LCAP, KC_LNUM or KC_LSCR instead in keymap
 */
#define LOCKING_SUPPORT_ENABLE
/* Locking resynchronize hack */
#define LOCKING_RESYNC_ENABLE

/* If defined, GRAVE_ESC will always act as ESC when CTRL is held.
 * This is useful for the Windows task manager shortcut (ctrl+shift+esc).
 */
// #define GRAVE_ESC_CTRL_OVERRIDE

/*
 * Force NKRO
 *
 * Force NKRO (nKey Rollover) to be enabled by default, regardless of the saved
 * state in the bootmagic EEPROM settings. (Note that NKRO must be enabled in
 * the makefile for this to work.)
 *
 * If forced on, NKRO can be disabled via magic key (default = LShift+RShift+N)
 * until the next keyboard reset.
 *
 * NKRO may prevent your keystrokes from being detected in the BIOS, but it is
 * fully operational during normal computer usage.
 *
 * For a less heavy-handed approach, enable NKRO via magic key (LShift+RShift+N)
 * or via bootmagic (hold SPACE+N while plugging in the keyboard). Once set by
 * bootmagic, NKRO mode will always be enabled until it is toggled again during
 * a power-up.
 *
 */
// #define FORCE_NKRO

/*
 * Feature disable options
 *  These options are also useful to firmware size reduction.
 */

/* disable debug print */
// #define NO_DEBUG

/* disable print */
// #define NO_PRINT

/* disable action features */
// #define NO_ACTION_LAYER
// #define NO_ACTION_TAPPING
// #define NO_ACTION_ONESHOT

/* disable these deprecated features by default */

/* Bootmagic Lite key configuration */
// #define BOOTMAGIC_LITE_ROW 0
// #define BOOTMAGIC_LITE_COLUMN 0
