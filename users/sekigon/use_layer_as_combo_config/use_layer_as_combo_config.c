// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H
#include "use_layer_as_combo_config.h"
#include "process_combo.h"

#ifndef COMBO_CONFIG_LAYER
#    define COMBO_CONFIG_LAYER (DYNAMIC_KEYMAP_LAYER_COUNT - 1)
#endif
static const uint8_t config_layer[] = {COMBO_CONFIG_LAYER};

#ifndef COMBO_COUNT
#    define COMBO_COUNT 32
combo_t  key_combos[COMBO_COUNT];
uint16_t COMBO_LEN;
#else
extern combo_t  key_combos[COMBO_COUNT];
extern uint16_t COMBO_LEN;
#endif

uint16_t combo_members[sizeof(config_layer) * MATRIX_ROWS * MATRIX_COLS / 2];

static uint16_t get_keycode(uint16_t config_key_idx) {
    uint8_t  layer = config_key_idx / (MATRIX_COLS * MATRIX_ROWS);
    uint8_t  row   = config_key_idx / MATRIX_COLS;
    uint8_t  col   = config_key_idx % MATRIX_COLS;
    keypos_t pos   = {.row = row, .col = col};
    return keymap_key_to_keycode(config_layer[layer], pos);
}

void convert_layer_to_combo(void) {
    const uint16_t key_cnt =
        (MATRIX_ROWS * MATRIX_COLS / 3) * sizeof(config_layer);
    uint16_t combo_count         = 0;
    uint16_t combo_member_write  = 0;
    uint8_t  combo_trigger_index = 0;

    for (uint16_t key_index = 0; key_index < key_cnt; key_index++) {
        uint16_t kc = get_keycode(key_index);

        if ((combo_trigger_index > MAX_COMBO_LENGTH) ||
            (combo_trigger_index < 2 && kc == KC_NO)) {
            break;
        } else if (kc == KC_NO) {
            key_combos[combo_count].keys =
                &combo_members[combo_member_write - combo_trigger_index];
            key_combos[combo_count].keycode =
                combo_members[combo_member_write - 1];
            combo_members[combo_member_write - 1] = COMBO_END;

            combo_trigger_index = 0;
            combo_count++;
        } else {
            combo_members[combo_member_write++] = kc;
            combo_trigger_index++;
        }
    }

    COMBO_LEN = combo_count;
}
