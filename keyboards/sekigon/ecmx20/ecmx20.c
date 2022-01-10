/* Copyright 2021 sekigon-gonnoc
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H

#include "ec_switch_matrix.h"
#include "eeprom.h"
#include "eeconfig.h"

extern rgblight_config_t rgblight_config;

static const uint32_t eeconfig_kb_magic = (((uint32_t)VENDOR_ID) << 16) | (PRODUCT_ID);
static void update_ecs_threshold(uint16_t low, uint16_t high);

void keyboard_post_init_kb() {
    debug_enable = true;
    // debug_matrix = true;

    // led_on();
    keyboard_post_init_user();
}

void keyboard_pre_init_kb(void) {
    // Turn on extern circuit
    setPinOutput(F7);
    writePinHigh(F7);

    if(eeconfig_read_kb() != eeconfig_kb_magic) {
        eeconfig_init();

        eeconfig_init_via();
    }
}

void eeconfig_init_kb(void) {
    eeconfig_update_kb(eeconfig_kb_magic);

    // reset threshold
    update_ecs_threshold(LOW_THRESHOLD, HIGH_THRESHOLD);

    eeconfig_init_user();
}

static void via_custom_lighting_get_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_rgblight_brightness: {
            value_data[0] = rgblight_get_val();
            break;
        }
        case id_qmk_rgblight_effect: {
            value_data[0] = rgblight_get_mode();
            break;
        }
        case id_qmk_rgblight_effect_speed: {
            value_data[0] = rgblight_get_speed();
            break;
        }
        case id_qmk_rgblight_color: {
            value_data[0] = rgblight_get_hue();
            value_data[1] = rgblight_get_sat();
            break;
        }
    }
}

static void via_custom_lighting_set_value(uint8_t *data) {
    uint8_t *value_id   = &(data[0]);
    uint8_t *value_data = &(data[1]);
    switch (*value_id) {
        case id_qmk_rgblight_brightness: {
            rgblight_sethsv_noeeprom(rgblight_get_hue(), rgblight_get_sat(), value_data[0]);
            break;
        }
        case id_qmk_rgblight_effect: {
            rgblight_mode_noeeprom(value_data[0]);
            if (value_data[0] == 0) {
                rgblight_disable_noeeprom();
            } else {
                rgblight_enable_noeeprom();
            }
            break;
        }
        case id_qmk_rgblight_effect_speed: {
            rgblight_set_speed_noeeprom(value_data[0]);
            break;
        }
        case id_qmk_rgblight_color: {
            rgblight_sethsv_noeeprom(value_data[0], value_data[1], rgblight_get_val());
            break;
        }
    }
}

static void update_ecs_threshold(uint16_t low, uint16_t high) {
    eeprom_update_word((uint16_t *)EEPROM_ECS_THRESHOLD_ADDR, low);
    eeprom_update_word((uint16_t *)(EEPROM_ECS_THRESHOLD_ADDR + 2), high);
}

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
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

                ecsm_config_t config;
                if (low_th == 0xffff && high_th == 0xffff) {
                    // Reset threshold
                    config.low_threshold  = LOW_THRESHOLD;
                    config.high_threshold = HIGH_THRESHOLD;
                } else {
                    // Update threshold
                    config.low_threshold  = low_th;
                    config.high_threshold = high_th;
                }
                ecsm_init(&config);
                update_ecs_threshold(config.low_threshold, config.high_threshold);
            }
            break;

        case id_lighting_set_value:
            via_custom_lighting_set_value(value_data);

            break;
        case id_lighting_get_value:
            via_custom_lighting_get_value(value_data);

            break;
        case id_lighting_save:
            // Save rgblight config per layer
            eeprom_update_dword((uint32_t *)(VIA_RGBLIGHT_USER_ADDR + 4 * layer), rgblight_config.raw);
            eeconfig_update_rgblight_current();
            break;
        default:
            break;
    }
}

