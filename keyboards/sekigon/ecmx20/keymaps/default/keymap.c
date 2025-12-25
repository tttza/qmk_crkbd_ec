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

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0]=LAYOUT(
                                   KC_MUTE, KC_VOLD, KC_VOLU,
        TO(1),   KC_SLSH, KC_ASTR, KC_BSPC,
        KC_7,    KC_8,    KC_9,    KC_MINS,
        KC_4,    KC_5,    KC_6,    KC_PLUS,
        KC_1,    KC_2,    KC_3,    KC_ENT,
        KC_0,    KC_0,    KC_DOT,  KC_ENT
        ),
    [1]=LAYOUT(
                                   KC_MEDIA_PLAY_PAUSE, KC_MEDIA_PREV_TRACK, KC_MEDIA_NEXT_TRACK,
        TO(0),   KC_KP_SLASH, KC_KP_ASTERISK, KC_KP_MINUS,
        KC_KP_7,    KC_KP_8,    KC_KP_9,    KC_KP_PLUS,
        KC_KP_4,    KC_KP_5,    KC_KP_6,    KC_KP_PLUS,
        KC_KP_1,    KC_KP_2,    KC_KP_3,    KC_KP_ENTER,
        KC_KP_0,    KC_KP_0,    KC_KP_DOT,  KC_KP_ENTER
        ),
};
// clang-format on

// Override to avoid messy initial keymap show on VIA
// Patch to dynamic_keymap.c is required
void dynamic_keymap_reset(void) {
    uint8_t defined_layer = (sizeof(keymaps) / sizeof(keymaps[0][0][0])) / MATRIX_ROWS / MATRIX_COLS;

    for (int layer = 0; layer < DYNAMIC_KEYMAP_LAYER_COUNT; layer++) {
        for (int row = 0; row < MATRIX_ROWS; row++) {
            for (int column = 0; column < MATRIX_COLS; column++) {
                if (layer < defined_layer) {
                    dynamic_keymap_set_keycode(layer, row, column, pgm_read_word(&keymaps[layer][row][column]));
                } else {
                    // Copy layer 0 to undefined layers
                    dynamic_keymap_set_keycode(layer, row, column, pgm_read_word(&keymaps[0][row][column]));
                }
            }
        }
    }
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    keypos_t key;

    if (index == 0) {
        if (clockwise) {
            key.row = 5;
            key.col = 2;
        } else {
            key.row = 5;
            key.col = 1;
        }
        uint8_t  layer   = layer_switch_get_layer(key);
        uint16_t keycode = keymap_key_to_keycode(layer, key);

        tap_code16(keycode);
    }
    return false;
}

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);

    if (layer < DYNAMIC_KEYMAP_LAYER_COUNT) {
        rgblight_update_dword(eeprom_read_dword((const uint32_t *)(VIA_RGBLIGHT_USER_ADDR + 4 * layer)));
    }

    return state;
}


bool process_record_user(uint16_t keycode, keyrecord_t *record) { return true; }

void keyboard_post_init_user() { layer_state_set_user(layer_state); }
