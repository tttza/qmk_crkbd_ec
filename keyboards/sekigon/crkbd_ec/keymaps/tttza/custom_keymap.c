#include QMK_KEYBOARD_H

#include "custom_keymap.h"

#include "eeprom.h"
#include "eeconfig.h"
#include "twpair_on_jis.h"

user_config_t user_config = {};

// Translate macro playback the same way as physical key input (US/JIS toggle aware)
static uint16_t lang_keycode(uint16_t keycode) {
    if (user_config.jis) {
        return us_to_jis_keycode(keycode);
    }
    return keycode;
}

void tap_code16_lang(uint16_t keycode) { tap_code16(lang_keycode(keycode)); }

// Ensure send_string macros also pass through US/JIS translation
void send_string_apply_keymap(uint8_t *keycode, bool *is_shifted, bool *is_altgred) {
    (void)is_altgred;

    if (!user_config.jis) return;

    uint16_t code = *keycode;
    if (*is_shifted) {
        code |= QK_LSFT;
    }

    uint16_t translated = us_to_jis_keycode(code);

    // Rebuild shift from the translated keycode
    *is_shifted = (translated & QK_LSFT) || (translated & QK_RSFT);
    *keycode    = translated & 0xFF;
}

void load_persistent(void) { user_config.raw = eeconfig_read_user(); }

void save_persistent(void) { eeconfig_update_user(user_config.raw); }

void eeconfig_init_user(void) {
    set_keyboard_lang_to_jis(true);
    save_persistent();
}

void keyboard_post_init_user(void) { load_persistent(); }

void set_keyboard_lang_to_jis(bool set_jis) {
    if (user_config.jis == set_jis) {
        return;
    }
    if (set_jis) {
        user_config.jis = 1;
    } else {
        user_config.jis = 0;
    }
    save_persistent();
}
