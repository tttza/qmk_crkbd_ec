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

#include "custom_keymap.h"
#include "eeconfig.h"

// Compatibility aliases for renamed keycodes
#ifndef RESET
#    define RESET QK_BOOT
#endif
#ifndef RGB_TOG
#    define RGB_TOG RM_TOGG
#endif
#ifndef RGB_MOD
#    define RGB_MOD RM_NEXT
#endif
#ifndef RGB_HUI
#    define RGB_HUI RM_HUEU
#endif
#ifndef RGB_HUD
#    define RGB_HUD RM_HUED
#endif
#ifndef RGB_SAI
#    define RGB_SAI RM_SATU
#endif
#ifndef RGB_SAD
#    define RGB_SAD RM_SATD
#endif
#ifndef RGB_VAI
#    define RGB_VAI RM_VALU
#endif
#ifndef RGB_VAD
#    define RGB_VAD RM_VALD
#endif
#ifndef KC_MHEN
#    define KC_MHEN JP_MHEN
#endif
#ifndef KC_HENK
#    define KC_HENK JP_HENK
#endif

#if defined(RGBLIGHT_ENABLE)
#    include "rgblight.h"
#elif defined(RGB_MATRIX_ENABLE)
#    include "rgb_matrix.h"
extern rgb_config_t rgb_matrix_config;
#endif

#include "caps_word.h"
#include "select_word.h"
#include "twpair_on_jis.h"
#include "keymap_extras/keymap_japanese.h"
#ifdef LEADER_ENABLE
#    include "process_keycode/process_leader.h"
#endif

static void tap_pair_inner(uint16_t left, uint16_t right) {
    tap_code16_lang(left);
    tap_code16_lang(right);
    tap_code16(KC_LEFT);
}

#ifdef CONSOLE_ENABLE
#    include "print.h"
#endif

enum layer_number {
    _QWERTY = 0,
    _LOWER,
    _RAISE,
    _ADJUST,
};

// Use USER keycodes so Remap/VIA can display them
// Keep existing 2–5 assignments; new ones start at USER10 for clarity
enum custom_keycodes {
    CK_EnJIS       = QK_USER_2,
    CK_EnUS        = QK_USER_3,
    WSEL           = QK_USER_4,
    CK_HAND_SWAP   = QK_USER_5,
    CK_PAREN_INNER = QK_USER_10,  // "()"
    CK_BRKT_INNER  = QK_USER_11,  // "[]"
    CK_BRCE_INNER  = QK_USER_12,  // "{}"
    CK_ANG_INNER   = QK_USER_13,  // "<>"
    CK_SQUO_INNER  = QK_USER_14,  // "''"
    CK_DQUO_INNER  = QK_USER_15,  // "\"\""
};

#define LOWER LT(_LOWER, KC_MHEN)
#define RAISE LT(_RAISE, KC_HENK)

// clang-format off
const uint16_t keymaps[DYNAMIC_KEYMAP_LAYER_COUNT][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_MINS,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_ESC,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                  KC_LGUI, LT(_LOWER, KC_MHEN), LSFT_T(KC_SPC),  LALT_T(KC_ENT), LT(_RAISE, KC_HENK), KC_BSPC
                                      //`--------------------------'  `--------------------------'
  ),

  [_LOWER] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,   KC_0, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_F6,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,  KC_F12,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                  KC_LGUI, _______,  KC_SPC,     LGUI(KC_F13), _______, KC_DEL
                                      //`--------------------------'  `--------------------------'
  ),

  [_RAISE] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
    KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_LCTL, XXXXXXX, XXXXXXX, KC_F15, XXXXXXX,  KC_PSCR,                      KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, KC_HOME,  KC_END,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
    KC_LSFT, CK_SQUO_INNER, CK_DQUO_INNER, CK_ANG_INNER, CK_BRKT_INNER, CK_PAREN_INNER,                     XXXXXXX, XXXXXXX, LALT_T(KC_ENT), WSEL, KC_PGUP, KC_PGDN,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                  KC_LGUI, _______, _______,     KC_ENT, _______, WSEL
                                      //`--------------------------'  `--------------------------'
  ),

  [_ADJUST] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        QK_BOOT, CK_HAND_SWAP,  AS_TOGG, XXXXXXX, XXXXXXX, XXXXXXX,                    XXXXXXX, CK_EnUS,  CW_TOGG, XXXXXXX, CK_HAND_SWAP, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        RM_TOGG,   RM_HUEU,   RM_SATU,   RM_VALU, XXXXXXX, XXXXXXX,                    QK_REBOOT, CK_EnJIS, KC_MUTE,  KC_VOLU,  KC_VOLD, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
        RM_NEXT,   RM_HUED,   RM_SATD,   RM_VALD, QK_MACRO_1, QK_MACRO_0,              XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, SE_UNLK,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                  KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  )
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    state = update_tri_layer_state(state, _RAISE, _LOWER, _ADJUST);

    return state;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (!process_caps_word(keycode, record)) {
        return false;
    }
    if (!process_select_word(keycode, record, WSEL)) {
        return false;
    }
    switch (keycode) {
        case CK_EnJIS:
            set_keyboard_lang_to_jis(true);
            return false;
        case CK_EnUS:
            set_keyboard_lang_to_jis(false);
            return false;
        case CK_HAND_SWAP:
            if (record->event.pressed) {
                bool is_left = eeconfig_read_handedness();
                eeconfig_update_handedness(!is_left);
                soft_reset_keyboard();
            }
            return false;
        case CK_PAREN_INNER:
            if (record->event.pressed) {
                // Inserts "()" and steps inside
                tap_pair_inner(KC_LPRN, KC_RPRN);
            }
            return false;
        case CK_BRKT_INNER:
            if (record->event.pressed) {
                // Inserts "[]" and steps inside
                tap_pair_inner(KC_LBRC, KC_RBRC);
            }
            return false;
        case CK_BRCE_INNER:
            if (record->event.pressed) {
                // Inserts "{}" and steps inside
                tap_pair_inner(KC_LCBR, KC_RCBR);
            }
            return false;
        case CK_DQUO_INNER:
            if (record->event.pressed) {
                // Inserts "\"\"" and steps inside
                tap_pair_inner(KC_DQT, KC_DQT);
            }
            return false;
        case CK_SQUO_INNER:
            if (record->event.pressed) {
                // Inserts "''" and steps inside
                tap_pair_inner(KC_QUOT, KC_QUOT);
            }
            return false;
        case CK_ANG_INNER:
            if (record->event.pressed) {
                // Inserts "<>" and steps inside
                tap_pair_inner(LSFT(KC_COMMA), LSFT(KC_DOT));
            }
            return false;
        default:
            if (user_config.jis) {
                if (leader_is_active()) {
                    return true;
                }
                return twpair_on_jis(keycode, record);
            } else {
                return true;
            }
    }
}
