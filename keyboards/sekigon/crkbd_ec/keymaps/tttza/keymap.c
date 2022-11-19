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
#include "pico_cdc.h"
#include "eeprom.h"

#if defined(RGBLIGHT_ENABLE)
#include "rgblight.h"
#elif defined(RGB_MATRIX_ENABLE)
#include "rgb_matrix.h"
extern rgb_config_t rgb_matrix_config;
#endif

#include "keymap_extras/keymap_jp.h"
#include "twpair_on_jis.h"
#include "custom_keymap.h"

user_config_t user_config = {};

enum layer_number {
  _QWERTY = 0,
  _LOWER,
  _RAISE,
  _ADJUST,
};

enum custom_keycodes {
  CK_EnJIS = SAFE_RANGE,
  CK_EnUS,
  LOWER,
  RAISE
};

// clang-format off
const uint16_t keymaps[DYNAMIC_KEYMAP_LAYER_COUNT][MATRIX_ROWS][MATRIX_COLS] = {
  [_QWERTY] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,                         KC_Y,    KC_U,    KC_I,    KC_O,   KC_P,  KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL,    KC_A,    KC_S,    KC_D,    KC_F,    KC_G,                         KC_H,    KC_J,    KC_K,    KC_L, KC_SCLN, KC_QUOT,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT,    KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,                         KC_N,    KC_M, KC_COMM,  KC_DOT, KC_SLSH,  KC_ESC,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                  KC_LGUI,   LOWER,  LSFT_T(KC_SPC),  CTL_T(KC_ENT),   RAISE, KC_RALT
                                      //`--------------------------'  `--------------------------'

  ),

  [_LOWER] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,                         KC_6,    KC_7,    KC_8,    KC_9,    KC_0, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_LEFT, KC_DOWN,   KC_UP,KC_RIGHT, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  [_RAISE] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
       KC_TAB, KC_EXLM,   KC_AT, KC_HASH,  KC_DLR, KC_PERC,                      KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_BSPC,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LCTL, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_MINS,  KC_EQL, KC_LBRC, KC_RBRC, KC_BSLS,  KC_GRV,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      KC_LSFT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      KC_UNDS, KC_PLUS, KC_LCBR, KC_RCBR, KC_PIPE, KC_TILD,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______, KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  ),

  [_ADJUST] = LAYOUT(
  //,-----------------------------------------------------.                    ,-----------------------------------------------------.
        RESET, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,                      XXXXXXX, CK_EnUS, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_TOG, RGB_HUI, RGB_SAI, RGB_VAI, XXXXXXX, XXXXXXX,                      XXXXXXX,CK_EnJIS, KC_MUTE, KC_VOLU, KC_VOLD, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------|                    |--------+--------+--------+--------+--------+--------|
      RGB_MOD, RGB_HUD, RGB_SAD, RGB_VAD, XXXXXXX, XXXXXXX,                      XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  //|--------+--------+--------+--------+--------+--------+--------|  |--------+--------+--------+--------+--------+--------+--------|
                                          KC_LGUI, _______,  KC_SPC,     KC_ENT, _______, KC_RALT
                                      //`--------------------------'  `--------------------------'
  )
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    uint8_t layer = get_highest_layer(state);

    if (layer < DYNAMIC_KEYMAP_LAYER_COUNT) {
#if defined(RGBLIGHT_ENABLE)
        rgblight_update_dword(eeprom_read_dword((const uint32_t *)(VIA_RGBLIGHT_USER_ADDR + 4 * layer)));
#elif defined(RGB_MATRIX_ENABLE)
        rgb_matrix_config.raw = eeprom_read_dword((const uint32_t *)(VIA_RGBLIGHT_USER_ADDR + 4 * layer));
#endif
    }

    return state;
}



static bool dprint_matrix = false;

void pico_cdc_on_disconnect(void) { dprint_matrix = false; }

bool pico_cdc_receive_kb(uint8_t const *buf, uint32_t cnt) {
    if (cnt > 0 && buf[0] == 'e') {
        dprint_matrix ^= true;
        return false;
    }
    return true;
}

void matrix_scan_user(void) {
    static int cnt = 0;
    if (dprint_matrix && cnt++ == 30) {
        cnt = 0;
        ecsm_dprint_matrix();
    }
}

void load_persistent(void) {
    user_config.raw = eeconfig_read_user();
}

void save_persistent(void) {
    eeconfig_update_user(user_config.raw);
}

void eeconfig_init_user(void) {
    save_persistent();
}

void keyboard_post_init_user(void) {
    layer_state_set_user(layer_state);
    if (is_keyboard_master()) {
        load_persistent();
    }
}

void set_keyboard_lang_to_jis(bool set_jis){
    if ( user_config.jis == set_jis){ return; }
    if (set_jis){
        user_config.jis = 1;
    } else {
        user_config.jis = 0;
    }
    save_persistent();
}


// ref: https://gist.github.com/okapies/5d13a174cbb13ce34dbd9faede9d0b71#file-keymap-c-L99-L164
static bool lower_pressed = false;
static bool lower_cmb_pressed = false;
static uint16_t lower_pressed_time = 0;
static bool raise_pressed = false;
static bool raise_cmb_pressed = false;
static uint16_t raise_pressed_time = 0;
bool process_lower(uint16_t keycode, keyrecord_t *record){
     if (record->event.pressed) {
        lower_pressed = true;
        lower_cmb_pressed = false;
        lower_pressed_time = record->event.time;

        layer_on(_LOWER);
      } else {
        layer_off(_LOWER);

        if (!lower_cmb_pressed && lower_pressed && (TIMER_DIFF_16(record->event.time, lower_pressed_time) < TAPPING_TERM)) {
          register_code(KC_MHEN);
          unregister_code(KC_MHEN);
        //   register_code(KC_LANG2); // for macOS
        //   unregister_code(KC_LANG2);
        }
        lower_pressed = false;
      }
      return false;
}
bool process_raise(uint16_t keycode, keyrecord_t *record){
      if (record->event.pressed) {
        raise_pressed = true;
        raise_cmb_pressed = false;
        raise_pressed_time = record->event.time;

        layer_on(_RAISE);
      } else {
        layer_off(_RAISE);

        if (!raise_cmb_pressed && raise_pressed && (TIMER_DIFF_16(record->event.time, raise_pressed_time) < TAPPING_TERM)) {
          register_code(KC_HENK);
          unregister_code(KC_HENK);
        //   register_code(KC_LANG1); // for macOS
        //   unregister_code(KC_LANG1);
        }
        raise_pressed = false;
      }
      return false;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {

  switch (keycode) {
    case CK_EnJIS:
        set_keyboard_lang_to_jis(true);
        return false;
    case CK_EnUS:
        set_keyboard_lang_to_jis(false);
        return false;
    case LOWER:
        return process_lower(keycode, record);
    case RAISE:
        return process_raise(keycode, record);
    default:
      // NOTE: is redundant?
      if (lower_pressed) { lower_cmb_pressed = true; }
      if (raise_pressed) { raise_cmb_pressed = true; }
      if (user_config.jis){
          return twpair_on_jis(keycode, record);
      } else {
          return true;
      }
  }


 }
