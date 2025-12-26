// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "ec_switch_matrix.h"
#include "eeprom.h"
#include "eeconfig.h"
#include "raw_hid.h"
#include "quantum/nvm/eeprom/nvm_eeprom_eeconfig_internal.h"
#include "tusb.h"

// Align custom lighting command IDs with VIA custom commands
enum {
    id_lighting_set_value = id_custom_set_value,
    id_lighting_get_value = id_custom_get_value,
    id_lighting_save      = id_custom_save,
};

#ifndef CRKBD_EC_DEBUG_DEFAULT
#    define CRKBD_EC_DEBUG_DEFAULT 0
#endif

static uint32_t debug_heartbeat_timer = 0;
static bool     console_ready_logged  = false;

static void update_ecs_threshold(uint16_t low, uint16_t high);

void keyboard_post_init_kb() {
#if CRKBD_EC_DEBUG_DEFAULT
    debug_enable = true;
    debug_matrix = true;
#endif

    // Log master/hand detection after split_pre_init has run.
        dprintf("post_init master=%d left=%d\n", is_keyboard_master(),
            is_keyboard_left());

    // Force handedness to match the compiled half on every boot so the slave
    // cannot get stuck with stale EE_HANDS data from a previous flash.
    extern uint8_t handness;
    eeprom_update_byte(EECONFIG_HANDEDNESS, handness);

    keyboard_post_init_user();
}

void matrix_scan_kb(void) {
#if CRKBD_EC_DEBUG_DEFAULT
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
#endif
    matrix_scan_user();
}

extern uint8_t handness;
void           eeconfig_init_kb(void) {
    // reset threshold
    update_ecs_threshold(LOW_THRESHOLD, HIGH_THRESHOLD);
    eeprom_update_byte(EECONFIG_HANDEDNESS, handness);
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
#if defined(RGBLIGHT_ENABLE) || defined(RGB_MATRIX_ENABLE)
    uint8_t  layer = get_highest_layer(layer_state);
#endif
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

                ecsm_config_t config;
                if (low_th == 0xffff && high_th == 0xffff) {
                    // Reset threshold
                    config.low_threshold  = LOW_THRESHOLD;
                    config.high_threshold = HIGH_THRESHOLD;
                } else {
                    // Enforce sane ordering and range.
                    const uint16_t adc_max = 4095;
                    if (high_th <= low_th) {
                        // Keep previous thresholds to avoid dead keyboard.
                        ecsm_get_config(&config);
                        low_th  = config.low_threshold;
                        high_th = config.high_threshold;
                    }
                    if (low_th > adc_max) {
                        low_th = adc_max;
                    }
                    if (high_th > adc_max) {
                        high_th = adc_max;
                    }
                    // Update threshold
                    config.low_threshold  = low_th;
                    config.high_threshold = high_th;
                }
                ecsm_init(&config);
                update_ecs_threshold(config.low_threshold,
                                     config.high_threshold);
            }
            break;

        case id_lighting_set_value:
            via_custom_lighting_set_value(value_data);

            break;
        case id_lighting_get_value:
            via_custom_lighting_get_value(value_data);

            break;
        case id_lighting_save:
#if defined(RGBLIGHT_ENABLE) || defined(RGB_MATRIX_ENABLE)
            // Save rgblight config per layer
            eeprom_update_dword(
                (uint32_t *)(VIA_RGBLIGHT_USER_ADDR + 4 * layer),
                LIGHTING_CONFIG.raw);
            LIGHTING_UPDATE_EECONFIG();
#endif
            break;
        default:
            break;
    }
}

static bool handle_custom_lighting_command(uint8_t command_id, uint8_t *data,
                                           uint8_t length) {
    if (length < 3) {
        return false;
    }

    uint8_t channel    = data[1];
    uint8_t *value_ptr = &data[2];

    // New VIA custom-value format: [cmd, channel, value_id, value...]
    if (channel == id_custom_channel || channel == id_qmk_rgblight_channel ||
        channel == id_qmk_rgb_matrix_channel) {
        switch (command_id) {
            case id_custom_set_value:
                via_custom_lighting_set_value(value_ptr);
                return true;
            case id_custom_get_value:
                via_custom_lighting_get_value(value_ptr);
                return true;
            case id_custom_save:
#if defined(RGBLIGHT_ENABLE) || defined(RGB_MATRIX_ENABLE)
                eeprom_update_dword(
                    (uint32_t *)(VIA_RGBLIGHT_USER_ADDR +
                                 4 * get_highest_layer(layer_state)),
                    LIGHTING_CONFIG.raw);
                LIGHTING_UPDATE_EECONFIG();
#endif
                return true;
            default:
                break;
        }
    }

    // Legacy VIA 0x0009-style layout: [cmd, value_id, value...]
    if (length >= 2 &&
        channel >= id_qmk_rgblight_brightness &&
        channel <= id_qmk_rgblight_color) {
        value_ptr = &data[1];
        switch (command_id) {
            case id_custom_set_value:
                via_custom_lighting_set_value(value_ptr);
                return true;
            case id_custom_get_value:
                via_custom_lighting_get_value(value_ptr);
                return true;
            case id_custom_save:
#if defined(RGBLIGHT_ENABLE) || defined(RGB_MATRIX_ENABLE)
                eeprom_update_dword(
                    (uint32_t *)(VIA_RGBLIGHT_USER_ADDR +
                                 4 * get_highest_layer(layer_state)),
                    LIGHTING_CONFIG.raw);
                LIGHTING_UPDATE_EECONFIG();
#endif
                return true;
            default:
                break;
        }
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

void suspend_power_down_kb(void) {
    rgb_matrix_set_suspend_state(true);
    suspend_power_down_user();
}

void suspend_wakeup_init_kb(void) {
    rgb_matrix_set_suspend_state(false);
    suspend_wakeup_init_user();
}
#endif
