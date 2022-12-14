/* Copyright 2021 IFo Hancroft
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

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* Default Layer
    * *---------------------------------------------------------------------*                             *---------------------------------------------------------------------*
    * | `       | 1       | 2       | 3       | 4       | 5       | -       |                             | =       | 6       | 7       | 8       | 9       | 0       | Del     |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * | Tab     | Q       | W       | E       | R       | T       | [       |                             | ]       | Y       | U       | I       | O       | P       | \       |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * | ESC     | A       | S       | D       | F       | G       |         |                             |         | H       | J       | K       | L       | ;       | '       |
    * |---------+---------+---------+---------+---------+---------+---------*                             *---------+---------+---------+---------+---------+---------+---------|
    * | Shift   | Z       | X       | C       | V       | B       |                                                 | N       | M       | ,       | .       | /       | Shift   |
    * |---------+---------+---------+---------+---------+---------*                                                 *---------+---------+---------+---------+---------+---------|
    * | Ctrl    | Super   | Alt     | MO(3)   | MO(2)   |                                                                     | MO(2)   | Left    | Down    | Up      | Right   |
    * *-------------------------------------------------*                                                                     *-------------------------------------------------*
    *
    *                                                             *-------------------*         *-------------------*
    *                                                             |         | TG(1)   |         |         |         |
    *                                                   *---------+---------+---------|         |---------+---------+---------*
    *                                                   |         |         |         |         |         |         |         |
    *                                                   | Bckspc  | Enter   |---------|         |---------| Enter   | Space   |
    *                                                   |         |         | PgDn    |         | PgUp    |         |         |
    *                                                   *---------+---------+---------*         *---------+---------+---------*
    */

    LAYOUT_ergodox_pretty(
        KC_GRV,   KC_1,     KC_2,     KC_3,    KC_4,      KC_5,     KC_MINS,                                KC_EQL,   KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_DEL,
        KC_TAB,   KC_Q,     KC_W,     KC_E,    KC_R,      KC_T,     KC_LBRC,                                KC_RBRC,  KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_BSLS,
        KC_ESC,   KC_A,     KC_S,     KC_D,    KC_F,      KC_G,                                                       KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,
        KC_LSFT,  KC_Z,     KC_X,     KC_C,    KC_V,      KC_B,     KC_NO,                                  KC_NO,    KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,  KC_RSFT,
        KC_LCTL,  KC_LGUI,  KC_LALT,  MO(3),   MO(2),                                                                           MO(2),    KC_LEFT,  KC_DOWN,  KC_UP,    KC_RGHT,

                                                                    KC_NO,    TG(1),              KC_NO,    KC_NO,
                                                                              KC_NO,              KC_NO,
                                                          KC_BSPC,  KC_ENT,   KC_PGDN,            KC_PGUP,  KC_ENT,   KC_SPC
    ),

    /* Gaming Layer
    * *---------------------------------------------------------------------*                             *---------------------------------------------------------------------*
    * |         |         |         |         |         |         | 6       |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * |         |         |         |         |         |         |         |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * |         |         |         |         |         |         |         |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------*                             *---------+---------+---------+---------+---------+---------+---------|
    * |         |         |         |         |         |         |                                                 |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------*                                                 *---------+---------+---------+---------+---------+---------|
    * |         |         |         |         |         |                                                                     |         |         |         |         |         |
    * *-------------------------------------------------*                                                                     *-------------------------------------------------*
    *
    *                                                             *-------------------*         *-------------------*
    *                                                             |         |         |         |         |         |
    *                                                   *---------+---------+---------|         |---------+---------+---------*
    *                                                   |         |         |         |         |         |         |         |
    *                                                   | Space   |         |---------|         |---------|         |         |
    *                                                   |         |         |         |         |         |         |         |
    *                                                   *---------+---------+---------*         *---------+---------+---------*
    */

    LAYOUT_ergodox_pretty(
        _______,  _______,  _______,  _______,  _______,  _______,  KC_6,                                   _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,                                                    _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,                                                                        _______,  _______,  _______,  _______,  _______,

                                                                    _______,  _______,            _______,  _______,
                                                                              _______,            _______,
                                                          KC_SPC,   _______,  _______,            _______,  _______,  _______
    ),

    /* Function Keys Layer
    * *---------------------------------------------------------------------*                             *---------------------------------------------------------------------*
    * |         | F1      | F2      | F3      | F4      | F5      | F11     |                             | F12     | F6      | F7      | F8      | F9      | F10     |         |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * |         |         | Up      |         |         |         |         |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * |         | Left    | Down    | Right   |         |         |         |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------*                             *---------+---------+---------+---------+---------+---------+---------|
    * |         |         |         |         |         |         |                                                 |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------*                                                 *---------+---------+---------+---------+---------+---------|
    * |         |         |         |         |         |                                                                     |         |         |         |         |         |
    * *-------------------------------------------------*                                                                     *-------------------------------------------------*
    *
    *                                                             *-------------------*         *-------------------*
    *                                                             |         |         |         |         |         |
    *                                                   *---------+---------+---------|         |---------+---------+---------*
    *                                                   |         |         |         |         |         |         |         |
    *                                                   |         |         |---------|         |---------|         |         |
    *                                                   |         |         |         |         |         |         |         |
    *                                                   *---------+---------+---------*         *---------+---------+---------*
    */

    LAYOUT_ergodox_pretty(
        _______,  KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F11,                                 KC_F12,   KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   _______,
        _______,  _______,  KC_UP,    _______,  _______,  _______,  _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  KC_LEFT,  KC_DOWN,  KC_RGHT,  _______,  _______,                                                    _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,                                                                        _______,  _______,  _______,  _______,  _______,

                                                                    _______,  _______,            _______,  _______,
                                                                              _______,            _______,
                                                          _______,  _______,  _______,            _______,  _______,  _______
    ),

    /* CAD Layer
    * *---------------------------------------------------------------------*                             *---------------------------------------------------------------------*
    * | Del     | 0       | 9       | 8       | 7       | 6       | =       |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * | \       | P       | O       | I       | U       | Y       | ]       |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------|                             |---------+---------+---------+---------+---------+---------+---------|
    * | '       | ;       | L       | K       | J       | H       |         |                             |         |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------+---------*                             *---------+---------+---------+---------+---------+---------+---------|
    * |         | /       | .       | ,       | M       | N       |                                                 |         |         |         |         |         |         |
    * |---------+---------+---------+---------+---------+---------*                                                 *---------+---------+---------+---------+---------+---------|
    * |         |         |         |         |         |                                                                     |         |         |         |         |         |
    * *-------------------------------------------------*                                                                     *-------------------------------------------------*
    *
    *                                                             *-------------------*         *-------------------*
    *                                                             |         |         |         |         |         |
    *                                                   *---------+---------+---------|         |---------+---------+---------*
    *                                                   |         |         |         |         |         |         |         |
    *                                                   | Space   |         |---------|         |---------|         |         |
    *                                                   |         |         | PgUp    |         |         |         |         |
    *                                                   *---------+---------+---------*         *---------+---------+---------*
    */

    LAYOUT_ergodox_pretty(
        KC_DEL,   KC_0,     KC_9,     KC_8,     KC_7,     KC_6,     KC_EQL,                                 _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KC_BSLS,  KC_P,     KC_O,     KC_I,     KC_U,     KC_Y,     KC_RBRC,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,
        KC_QUOT,  KC_SCLN,  KC_L,     KC_K,     KC_J,     KC_H,                                                       _______,  _______,  _______,  _______,  _______,  _______,
        _______,  KC_SLSH,  KC_DOT,   KC_COMM,  KC_M,     KC_N,     _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,                                                                        _______,  _______,  _______,  _______,  _______,

                                                                    _______,  _______,            _______,  _______,
                                                                              _______,            _______,
                                                          KC_SPC,   _______,  KC_PGUP,            _______,  _______,  _______
    )

};
