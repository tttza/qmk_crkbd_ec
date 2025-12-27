/* Copyright 2018-2020 eswai <@eswai>
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

/* Translate US keycap symbols to the correct JIS outputs when the OS is set to
 * a JIS keyboard, keeping symbols aligned (e.g., Shift + 2 -> @). Translated
 * outputs do not repeat. */

#include QMK_KEYBOARD_H
#include "keymap_jp.h"

const uint16_t us2jis[][2] = {
    {KC_LPRN, JP_LPRN}, {KC_RPRN, JP_RPRN}, {KC_AT, JP_AT},
    {KC_LBRC, JP_LBRC}, {KC_RBRC, JP_RBRC}, {KC_LCBR, JP_LCBR},
    {KC_RCBR, JP_RCBR}, {KC_MINS, JP_MINS}, {KC_EQL, JP_EQL},
    {KC_BSLS, JP_BSLS}, {KC_SCLN, JP_SCLN}, {KC_QUOT, JP_QUOT},
    {KC_GRV, JP_GRV},   {KC_PLUS, JP_PLUS}, {KC_COLN, JP_COLN},
    {KC_UNDS, JP_UNDS}, {KC_PIPE, JP_PIPE}, {KC_DQT, JP_DQUO},
    {KC_ASTR, JP_ASTR}, {KC_TILD, JP_TILD}, {KC_AMPR, JP_AMPR},
    {KC_CIRC, JP_CIRC},
};

bool twpair_on_jis(uint16_t keycode, keyrecord_t *record) {
    if (!record->event.pressed) return true;

    // Track shift state across normal/oneshot/weak mods so Caps Word and OSM work.
    uint8_t  saved_mods     = get_mods();
    uint8_t  saved_oneshot  = get_oneshot_mods();
    uint8_t  saved_weak     = get_weak_mods();
    uint8_t  combined_mods  = saved_mods | saved_oneshot | saved_weak;
    bool     shifted        = combined_mods & MOD_MASK_SHIFT;
    uint16_t skeycode       = shifted ? (QK_LSFT | keycode) : keycode;

    // Avoid double-shift by temporarily clearing shift-like mods.
    del_mods(MOD_MASK_SHIFT);
    del_oneshot_mods(MOD_MASK_SHIFT);
    del_weak_mods(MOD_MASK_SHIFT);

    for (int i = 0; i < sizeof(us2jis) / sizeof(us2jis[0]); i++) {
        if (us2jis[i][0] == skeycode) {
            if ((us2jis[i][1] & QK_LSFT) == QK_LSFT ||
                (us2jis[i][1] & QK_RSFT) == QK_RSFT) {
                register_code(KC_LSFT);
                tap_code(us2jis[i][1]);
                unregister_code(KC_LSFT);
            } else {
                tap_code(us2jis[i][1]);
            }

            // Restore original modifier state (including oneshot/weak).
            set_mods(saved_mods);
            add_weak_mods(saved_weak);
            add_oneshot_mods(saved_oneshot);
            return false;
        }
    }

    set_mods(saved_mods);
    add_weak_mods(saved_weak);
    add_oneshot_mods(saved_oneshot);

    return true;
}

uint16_t us_to_jis_keycode(uint16_t keycode) {
    // Preserve modifier bits (e.g. LCTL(KC_AT)) while translating the base
    // keycode.
    uint16_t mods        = 0;
    uint16_t search_code = keycode;

    // Extract mods in the QK_MODS range; keep shift info to rebuild
    // search_code.
    if (keycode >= QK_MODS && keycode <= QK_MODS_MAX) {
        mods        = keycode & 0x1F00;  // ctrl/shift/alt/gui bits
        search_code = keycode & 0xFF;    // base keycode (0-255)
        if (mods & (QK_LSFT | QK_RSFT)) {
            search_code |=
                QK_LSFT;  // normalize to include shift in the base code
        }
    }

    uint16_t translated = search_code;

    for (int i = 0; i < sizeof(us2jis) / sizeof(us2jis[0]); i++) {
        if (us2jis[i][0] == search_code) {
            translated = us2jis[i][1];
            break;
        }
    }

    // Re-attach non-shift modifiers (ctrl/alt/gui) to the translated keycode.
    uint16_t mods_no_shift = mods & ~(QK_LSFT | QK_RSFT);
    return mods_no_shift | translated;
}
