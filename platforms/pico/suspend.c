// Legacy Pico suspend handling
// Copyright 2025
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdbool.h>
#include "matrix.h"
#include "action.h"
#include "suspend.h"
#include "timer.h"
#include "led.h"
#include "host.h"

#include "tusb.h"

#ifdef BACKLIGHT_ENABLE
#    include "backlight.h"
#endif

#ifdef AUDIO_ENABLE
#    include "audio.h"
#endif

#if defined(RGBLIGHT_SLEEP) && defined(RGBLIGHT_ENABLE)
#    include "rgblight.h"
#endif

extern matrix_row_t matrix_previous[MATRIX_ROWS];
static matrix_row_t wakeup_matrix[MATRIX_ROWS];

void suspend_idle(uint8_t time) {
}

__attribute__((weak)) void suspend_power_down_user(void) {}
__attribute__((weak)) void suspend_power_down_kb(void) { suspend_power_down_user(); }

void suspend_power_down(void) {
    if (!tud_suspended()) return;

    suspend_power_down_kb();

#ifndef NO_SUSPEND_POWER_DOWN
#    ifdef BACKLIGHT_ENABLE
    backlight_set(0);
#    endif

    uint8_t leds_off = 0;
#    if defined(BACKLIGHT_CAPS_LOCK) && defined(BACKLIGHT_ENABLE)
    if (is_backlight_enabled()) {
        leds_off |= (1 << USB_LED_CAPS_LOCK);
    }
#    endif
    led_set(leds_off);

#    ifdef AUDIO_ENABLE
    stop_all_notes();
#    endif

#    if defined(RGBLIGHT_SLEEP) && defined(RGBLIGHT_ENABLE)
    rgblight_suspend();
#    endif

#    if defined(WDT_vect)
    power_down(WDTO_15MS);
#    endif
#endif
}

__attribute__((weak)) void matrix_power_up(void) {}
__attribute__((weak)) void matrix_power_down(void) {}

void update_matrix_state_after_wakeup(void) {
    matrix_power_up();
    matrix_scan();
    matrix_power_down();

    for (uint8_t row = 0; row < MATRIX_ROWS; row++) {
        const matrix_row_t current_row = matrix_get_row(row);
        wakeup_matrix[row] |= current_row & ~matrix_previous[row];
        matrix_previous[row] |= current_row;
    }
}

bool keypress_is_wakeup_key(uint8_t row, uint8_t col) {
    return (wakeup_matrix[row] & ((matrix_row_t)1 << col));
}

void wakeup_matrix_handle_key_event(uint8_t row, uint8_t col, bool pressed) {
    if (!pressed) {
        wakeup_matrix[row] &= ~((matrix_row_t)1 << col);
    }
}

bool suspend_wakeup_condition(void) {
    matrix_power_up();
    matrix_scan();
    matrix_power_down();
    for (uint8_t r = 0; r < MATRIX_ROWS; r++) {
        if (matrix_get_row(r)) return true;
    }
    return false;
}

__attribute__((weak)) void suspend_wakeup_init_user(void) {}
__attribute__((weak)) void suspend_wakeup_init_kb(void) { suspend_wakeup_init_user(); }

void suspend_wakeup_init(void) {
    clear_keyboard();

#ifdef BACKLIGHT_ENABLE
    backlight_init();
#endif

#ifdef AUDIO_ENABLE
    audio_on();
#endif

#if defined(RGBLIGHT_SLEEP) && defined(RGBLIGHT_ENABLE)
    rgblight_wakeup();
#endif

    suspend_wakeup_init_kb();
}