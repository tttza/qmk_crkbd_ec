// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "ec_switch_matrix.h"
#include "eeprom.h"
#include "eeconfig.h"
#include "matrix.h"
#include "raw_hid.h"
#include "quantum/nvm/eeprom/nvm_eeprom_eeconfig_internal.h"
#include "transport.h"
#include "tusb.h"
#include "xiao_status_led.h"
#include "keymaps/tttza/custom_keymap.h"
#include "split_util.h"
#include <string.h>

#define ROWS_PER_HAND (MATRIX_ROWS / 2)

// Align custom lighting command IDs with VIA custom commands
enum {
    id_lighting_set_value = id_custom_set_value,
    id_lighting_get_value = id_custom_get_value,
    id_lighting_save      = id_custom_save,
};

#ifndef CRKBD_EC_DEBUG_DEFAULT
#    define CRKBD_EC_DEBUG_DEFAULT 0
#endif

static uint32_t debug_heartbeat_timer __attribute__((unused)) = 0;
static bool     console_ready_logged __attribute__((unused))  = false;
static uint32_t last_matrix_change_ms                        = 0;
static bool     stuck_key_alert                              = false;
static xiao_rgb_t stuck_key_color                            = {120, 120, 0};

extern user_config_t user_config;

extern matrix_row_t raw_matrix[MATRIX_ROWS];
extern matrix_row_t matrix[MATRIX_ROWS];

static matrix_row_t last_local_state[ROWS_PER_HAND] = {0};

static uint8_t local_row_offset(void) { return is_keyboard_left() ? 0 : ROWS_PER_HAND; }

// Return the lowest (physically bottom) active row on this half, or -1 if none.
static int8_t bottommost_active_row(const matrix_row_t *state) {
    int8_t row_found = -1;
    for (uint8_t r = 0; r < ROWS_PER_HAND; r++) {
        if (state[r]) {
            row_found = (int8_t)r;
        }
    }

    return row_found;
}

static xiao_rgb_t stuck_row_color(int8_t row) {
    static const xiao_rgb_t palette[ROWS_PER_HAND] = {
        {160, 0, 40},   // top row hint
        {120, 120, 0},  // middle row hint
        {0, 140, 140},  // bottom row hint
    };

    if (row >= 0 && row < (int8_t)ROWS_PER_HAND) {
        return palette[row];
    }

    return (xiao_rgb_t){120, 120, 0};
}

static void update_ecs_threshold(uint16_t low, uint16_t high);
void matrix_slave_scan_user(void);

static void run_user_scan_hooks(bool is_slave) {
    if (is_slave) {
        matrix_slave_scan_user();
    }

    matrix_scan_user();
}

static void crkbd_ec_matrix_scan(bool is_slave) {
#if CRKBD_EC_DEBUG_DEFAULT
    if (!is_slave) {
        if (!console_ready_logged && tud_ready()) {
            console_ready_logged = true;
            dprintf("console up master=%d left=%d\n", is_keyboard_master(),
                    is_keyboard_left());
        }

        if (timer_elapsed32(debug_heartbeat_timer) > 1000) {
            debug_heartbeat_timer = timer_read32();
            dprintf("alive master=%d left=%d\n", is_keyboard_master(),
                    is_keyboard_left());
        }
    }
#endif

    run_user_scan_hooks(is_slave);

    // Refresh layer/host LED state locally on both halves so the slave sees the
    // same indicators as the master.
    xiao_status_led_set_layer(layer_state);
    xiao_status_led_set_host_leds(host_keyboard_led_state());

    const uint8_t  row_offset = local_row_offset();
    matrix_row_t   local_state[ROWS_PER_HAND];
    for (uint8_t r = 0; r < ROWS_PER_HAND; r++) {
        local_state[r] = raw_matrix[row_offset + r] | matrix[row_offset + r];
    }

    // Stuck-key detection: if the local matrix is unchanged and non-empty
    // for longer than the threshold, raise an alert only on this half.
    const uint32_t now_ms      = timer_read32();
    const bool     raw_changed = memcmp(last_local_state, local_state, sizeof(last_local_state)) != 0;
    const bool     any_pressed = ({
        bool pressed = false;
        for (uint8_t r = 0; r < ROWS_PER_HAND; r++) {
            if (local_state[r]) {
                pressed = true;
                break;
            }
        }
        pressed;
    });

    if (raw_changed) {
        memcpy(last_local_state, local_state, sizeof(last_local_state));
        last_matrix_change_ms = now_ms;
        if (stuck_key_alert) {
            stuck_key_alert = false;
            xiao_status_led_set_status((xiao_rgb_t){0, 0, 0}, false);
        }
    } else if (any_pressed) {
        const uint32_t STUCK_MS = 5000;  // 5 seconds before flagging
        if (!stuck_key_alert && timer_elapsed32(last_matrix_change_ms) > STUCK_MS) {
            stuck_key_alert = true;
            const int8_t row_hint = bottommost_active_row(local_state);
            stuck_key_color       = stuck_row_color(row_hint);
            xiao_status_led_set_status(stuck_key_color, true);  // blink per-row color on NeoPixel
        }
    } else {
        // No keys active and no change: keep timer fresh to avoid stale alert.
        last_matrix_change_ms = now_ms;
        if (stuck_key_alert) {
            stuck_key_alert = false;
            xiao_status_led_set_status((xiao_rgb_t){0, 0, 0}, false);
        }
    }

    // User LED status priority (discrete LEDs): USB error (red) > split error (green) > US mode (blue).
    uint8_t user_status_mask = 0;
    bool    usb_ready        = is_keyboard_master() ? tud_ready() : true;
    bool    split_ok         = is_keyboard_master() ? is_transport_connected() : true;
    bool    us_mode          = !user_config.jis;

    if (!usb_ready) {
        user_status_mask = 0x01;  // red
    } else if (!split_ok) {
        user_status_mask = 0x02;  // green
    } else if (us_mode) {
        user_status_mask = 0x04;  // blue
    }

    xiao_status_led_set_alert(user_status_mask);

    // NeoPixel status priority: stuck-key alert > Caps Word indicator > off.
    if (stuck_key_alert) {
        xiao_status_led_set_status(stuck_key_color, true);
    } else if (is_caps_word_on()) {
        xiao_status_led_set_status((xiao_rgb_t){0, 80, 100}, false);
    } else {
        xiao_status_led_set_status((xiao_rgb_t){0, 0, 0}, false);
    }

    xiao_status_led_task();
}

static void apply_ec_thresholds(uint16_t low_th, uint16_t high_th) {
    ecsm_config_t config;
    ecsm_get_config(&config);

    if (low_th == 0xffff && high_th == 0xffff) {
        // Reset threshold to defaults.
        config.low_threshold  = LOW_THRESHOLD;
        config.high_threshold = HIGH_THRESHOLD;
    } else {
        const uint16_t adc_max = 4095;
        if (low_th > adc_max) {
            low_th = adc_max;
        }
        if (high_th > adc_max) {
            high_th = adc_max;
        }

        // Keep previous thresholds if ordering is invalid to avoid a dead matrix.
        if (high_th > low_th) {
            config.low_threshold  = low_th;
            config.high_threshold = high_th;
        }
    }

    ecsm_init(&config);
    update_ecs_threshold(config.low_threshold, config.high_threshold);
}

void keyboard_post_init_kb() {
#if CRKBD_EC_DEBUG_DEFAULT
    debug_enable = true;
    debug_matrix = true;
#endif

    xiao_status_led_init();
    xiao_status_led_set_layer(layer_state);
    xiao_status_led_set_host_leds(host_keyboard_led_state());
    xiao_status_led_set_alert(0);

    // Log master/hand detection after split_pre_init has run.
    dprintf("post_init master=%d left=%d\n", is_keyboard_master(),
            is_keyboard_left());

    keyboard_post_init_user();
}

void matrix_scan_kb(void) { crkbd_ec_matrix_scan(false); }

void matrix_slave_scan_kb(void) { crkbd_ec_matrix_scan(true); }

void           eeconfig_init_kb(void) {
    // reset threshold
    update_ecs_threshold(LOW_THRESHOLD, HIGH_THRESHOLD);
    eeconfig_init_user();
}

#if defined(RGBLIGHT_ENABLE)
#    define LIGHTING_GET_VAL rgblight_get_val
#    define LIGHTING_GET_MODE rgblight_get_mode
#    define LIGHTING_GET_SPEED rgblight_get_speed
#    define LIGHTING_GET_HUE rgblight_get_hue
#    define LIGHTING_GET_SAT rgblight_get_sat
#    define LIGHTING_SETHSV_NOEEPROM(h, s, v) rgblight_sethsv_noeeprom(h, s, v)
#    define LIGHTING_SETMODE_NOEEPROM(mode) rgblight_mode_noeeprom(mode)
#    define LIGHTING_DISABLE_NOEEPROM() rgblight_disable_noeeprom()
#    define LIGHTING_ENABLE_NOEEPROM() rgblight_enable_noeeprom()
#    define LIGHTING_SETSPEED_NOEEPROM(speed) rgblight_set_speed_noeeprom(speed)
#    define LIGHTING_UPDATE_EECONFIG() eeconfig_update_rgblight_current()
#    define LIGHTING_CONFIG rgblight_config
extern rgblight_config_t rgblight_config;
#elif defined(RGB_MATRIX_ENABLE)
#    define LIGHTING_GET_VAL rgb_matrix_get_val
#    define LIGHTING_GET_MODE rgb_matrix_get_mode
#    define LIGHTING_GET_SPEED rgb_matrix_get_speed
#    define LIGHTING_GET_HUE rgb_matrix_get_hue
#    define LIGHTING_GET_SAT rgb_matrix_get_sat
#    define LIGHTING_SETHSV_NOEEPROM(h, s, v) \
        rgb_matrix_sethsv_noeeprom(h, s, v)
#    define LIGHTING_SETMODE_NOEEPROM(mode) rgb_matrix_mode_noeeprom(mode)
#    define LIGHTING_DISABLE_NOEEPROM() rgb_matrix_disable_noeeprom()
#    define LIGHTING_ENABLE_NOEEPROM() rgb_matrix_enable_noeeprom()
#    define LIGHTING_SETSPEED_NOEEPROM(speed) \
        rgb_matrix_set_speed_noeeprom(speed)
#    define LIGHTING_UPDATE_EECONFIG() eeconfig_update_rgb_matrix(&rgb_matrix_config)
#    define LIGHTING_CONFIG rgb_matrix_config
extern rgb_config_t rgb_matrix_config;
#else
#    define LIGHTING_GET_VAL(...) 0
#    define LIGHTING_GET_MODE(...) 0
#    define LIGHTING_GET_SPEED(...) 0
#    define LIGHTING_GET_HUE(...) 0
#    define LIGHTING_GET_SAT(...) 0
#    define LIGHTING_SETHSV_NOEEPROM(h, s, v)
#    define LIGHTING_SETMODE_NOEEPROM(mode)
#    define LIGHTING_DISABLE_NOEEPROM()
#    define LIGHTING_ENABLE_NOEEPROM()
#    define LIGHTING_SETSPEED_NOEEPROM(speed)
#    define LIGHTING_UPDATE_EECONFIG()
#endif

static void save_lighting_for_layer(uint8_t layer) {
#if defined(RGBLIGHT_ENABLE) || defined(RGB_MATRIX_ENABLE)
    eeprom_update_dword((uint32_t *)(VIA_RGBLIGHT_USER_ADDR + 4 * layer),
                        LIGHTING_CONFIG.raw);
    LIGHTING_UPDATE_EECONFIG();
#else
    (void)layer;
#endif
}

static void via_custom_lighting_get_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_rgblight_brightness: {
            value_data[0] = LIGHTING_GET_VAL();
            break;
        }
        case id_qmk_rgblight_effect: {
            value_data[0] = LIGHTING_GET_MODE();
            break;
        }
        case id_qmk_rgblight_effect_speed: {
            value_data[0] = LIGHTING_GET_SPEED();
            break;
        }
        case id_qmk_rgblight_color: {
            value_data[0] = LIGHTING_GET_HUE();
            value_data[1] = LIGHTING_GET_SAT();
            break;
        }
    }
}

static void via_custom_lighting_set_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_rgblight_brightness: {
            LIGHTING_SETHSV_NOEEPROM(LIGHTING_GET_HUE(), LIGHTING_GET_SAT(),
                                     value_data[0]);
            break;
        }
        case id_qmk_rgblight_effect: {
            LIGHTING_SETMODE_NOEEPROM(value_data[0]);
            if (value_data[0] == 0) {
                LIGHTING_DISABLE_NOEEPROM();
            } else {
                LIGHTING_ENABLE_NOEEPROM();
            }
            break;
        }
        case id_qmk_rgblight_effect_speed: {
            LIGHTING_SETSPEED_NOEEPROM(value_data[0]);
            break;
        }
        case id_qmk_rgblight_color: {
            LIGHTING_SETHSV_NOEEPROM(value_data[0], value_data[1],
                                     LIGHTING_GET_VAL());
            break;
        }
    }
}

static bool process_lighting_command(uint8_t command_id, uint8_t *payload) {
    switch (command_id) {
        case id_custom_set_value:
            via_custom_lighting_set_value(payload);
            return true;
        case id_custom_get_value:
            via_custom_lighting_get_value(payload);
            return true;
        case id_custom_save:
            save_lighting_for_layer(get_highest_layer(layer_state));
            return true;
        default:
            return false;
    }
}

static void update_ecs_threshold(uint16_t low, uint16_t high) {
    eeprom_update_word((uint16_t *)EEPROM_ECS_THRESHOLD_ADDR, low);
    eeprom_update_word((uint16_t *)(EEPROM_ECS_THRESHOLD_ADDR + 2), high);
}

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (length < 6) {
        return;
    }

    uint8_t *command_id = &(data[0]);
    uint8_t *value_data = &(data[1]);
    uint8_t  layer      = get_highest_layer(layer_state);
    switch (*command_id) {
        case id_get_keyboard_value:
            if (data[1] == 0xec) {
                ecsm_config_t config;
                ecsm_get_config(&config);
                data[2] = (config.low_threshold >> 8) & 0xff;
                data[3] = (config.low_threshold) & 0xff;
                data[4] = (config.high_threshold >> 8) & 0xff;
                data[5] = (config.high_threshold) & 0xff;
            }
            break;

        case id_set_keyboard_value:
            if (data[1] == 0xec) {
                uint16_t low_th  = (((uint16_t)data[2]) << 8) | data[3];
                uint16_t high_th = (((uint16_t)data[4]) << 8) | data[5];
                apply_ec_thresholds(low_th, high_th);
            }
            break;

        case id_lighting_set_value:
            via_custom_lighting_set_value(value_data);

            break;
        case id_lighting_get_value:
            via_custom_lighting_get_value(value_data);

            break;
        case id_lighting_save:
            save_lighting_for_layer(layer);
            break;
        default:
            break;
    }
}

static bool handle_custom_lighting_command(uint8_t command_id, uint8_t *data,
                                           uint8_t length) {
    if (length < 2) {
        return false;
    }

    uint8_t channel    = data[1];
    uint8_t *value_ptr = &data[1];

    // New VIA custom-value format: [cmd, channel, value_id, value...]
    if (length >= 3 &&
        (channel == id_custom_channel || channel == id_qmk_rgblight_channel ||
         channel == id_qmk_rgb_matrix_channel)) {
        value_ptr = &data[2];
        return process_lighting_command(command_id, value_ptr);
    }

    // Legacy VIA 0x0009-style layout: [cmd, value_id, value...]
    if (channel >= id_qmk_rgblight_brightness &&
        channel <= id_qmk_rgblight_color) {
        return process_lighting_command(command_id, value_ptr);
    }

    return false;
}

bool via_command_kb(uint8_t *data, uint8_t length) {
    if (length < 2) {
        return false;
    }

    uint8_t command_id = data[0];

    if ((command_id == id_get_keyboard_value || command_id == id_set_keyboard_value) &&
        length >= 6 && data[1] == 0xec) {
        raw_hid_receive_kb(data, length);
        raw_hid_send(data, length);
        return true;
    }

    if (command_id == id_custom_set_value || command_id == id_custom_get_value ||
        command_id == id_custom_save) {
        if (handle_custom_lighting_command(command_id, data, length)) {
            raw_hid_send(data, length);
            return true;
        }
    }

    return false;
}

layer_state_t layer_state_set_kb(layer_state_t state) {
    layer_state_t updated = layer_state_set_user(state);
    xiao_status_led_set_layer(updated);
    return updated;
}

bool led_update_kb(led_t led_state) {
    xiao_status_led_set_host_leds(led_state);
    return led_update_user(led_state);
}

#ifdef RGB_MATRIX_ENABLE
led_config_t g_led_config = {
    {
        {5, 4, 3, 2, 1, 0, 15},
        {6, 7, 8, 9, 10, 11, 14},
        {20, 19, 18, 17, 16, 12, 13},
        {26, 25, 24, 23, 22, 21, 36},
        {27, 28, 29, 30, 31, 32, 35},
        {41, 40, 39, 38, 37, 33, 34},
    },
    {{85, 4},   {68, 2},   {50, 0},   {33, 3},   {16, 7},   {0, 7},
     {0, 24},   {16, 24},  {33, 20},  {50, 17},  {68, 19},  {85, 21},
     {85, 39},  {95, 63},  {80, 58},  {60, 55},  {68, 37},  {50, 35},
     {33, 38},  {16, 42},  {0, 42},   {139, 4},  {156, 2},  {174, 0},
     {191, 3},  {208, 7},  {224, 7},  {224, 24}, {208, 24}, {191, 20},
     {174, 17}, {156, 19}, {139, 21}, {139, 39}, {129, 63}, {144, 58},
     {164, 55}, {156, 37}, {174, 35}, {191, 38}, {208, 42}, {224, 42}},
    {
        4, 4, 4, 4, 4, 1, 1, 4, 4, 4, 4, 4, 1, 1, 1, 4, 4, 4, 4, 4, 1,
        4, 4, 4, 4, 4, 1, 1, 4, 4, 4, 4, 4, 1, 1, 1, 4, 4, 4, 4, 4, 1,
    }};

#endif

void suspend_power_down_kb(void) {
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_set_suspend_state(true);
#endif
    xiao_status_led_suspend();
    suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
#ifdef RGB_MATRIX_ENABLE
    rgb_matrix_set_suspend_state(false);
#endif
    xiao_status_led_wakeup();
    suspend_wakeup_init_user();
}
