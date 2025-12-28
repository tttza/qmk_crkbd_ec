#pragma once

#include <stdbool.h>
#include <stdint.h>

typedef union {
    uint32_t raw;
    struct {
        bool jis : 1;
    };
} user_config_t;

extern user_config_t user_config;

void load_persistent(void);
void save_persistent(void);
void set_keyboard_lang_to_jis(bool set_jis);
void tap_code16_lang(uint16_t keycode);
void send_string_apply_keymap(uint8_t *keycode, bool *is_shifted, bool *is_altgred);
bool leader_is_active(void);
