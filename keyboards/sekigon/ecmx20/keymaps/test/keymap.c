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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [0]={{KC_NO}}
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    return true;
}

bool encoder_update_user(uint8_t index, bool clockwise) {
    dprintf("encoder:%d,%d\n", index, clockwise);
    return false;
}

void matrix_scan_user(void) {
    static int cnt = 0;
    if (cnt++ == 30) {
        cnt = 0;
        ecsm_dprint_matrix();
    }
}

#include "rgblight.h"
void keyboard_post_init_user(void) {
    rgblight_mode_noeeprom(RGBLIGHT_MODE_RGB_TEST);
}
