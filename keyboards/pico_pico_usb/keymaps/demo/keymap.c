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


#include "via.h"

const uint16_t PROGMEM keymaps[4][MATRIX_ROWS][MATRIX_COLS] = {
    {{USER00, USER01, USER02, USER03}, {USER04, USER05, USER06, USER07}, {USER08, USER09, USER10, USER11}}};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        switch (keycode) {
            case USER00:
                send_string("This ");
                return false;
            case USER01:
                send_string("is ");
                return false;
            case USER02:
                send_string("a ");
                return false;
            case USER03:
                send_string("keyboard.\n");
                return false;
            case USER04:
                send_string("split keyboard ");
                return false;
            case USER05:
                send_string("connected ");
                return false;
            case USER06:
                send_string("by ");
                return false;
            case USER07:
                send_string("USB using PIO!!\n");
                return false;
            case USER08:
                send_string("triple split keyboard ");
                return false;
            case USER09:
                return false;
            case USER10:
                return false;
            case USER11:
                return false;
        }
    }
    return true;
}
