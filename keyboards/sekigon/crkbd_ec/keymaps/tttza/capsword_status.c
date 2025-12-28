#include QMK_KEYBOARD_H

#include "xiao_status_service.h"

// Indicate Caps Word with the GP12 status pixel only.
void caps_word_set_user(bool active) {
    if (active) {
        xiao_status_service_set(XIAO_STATUS_SLOT_CAPS_WORD, (xiao_rgb_t){0, 80, 100}, false);
    } else {
        xiao_status_service_clear(XIAO_STATUS_SLOT_CAPS_WORD);
    }
}

// Keep caps word active for letters, digits, and common separators.
bool caps_word_press_user(uint16_t keycode) {
    switch (keycode) {
        case KC_A ... KC_Z:
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;
        case KC_1 ... KC_0:
        case KC_BSPC:
        case KC_DEL:
            return true;
        case KC_MINS:
        case KC_UNDS:
            add_weak_mods(MOD_BIT(KC_LSFT));
            return true;
        default:
            return false;
    }
}
