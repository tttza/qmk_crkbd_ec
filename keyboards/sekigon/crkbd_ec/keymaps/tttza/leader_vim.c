#include QMK_KEYBOARD_H
#include "leader.h"
#include "process_keycode/process_leader.h"
#include "xiao_status_service.h"
#include "keymap_extras/keymap_japanese.h"

#ifndef KC_MHEN
#    define KC_MHEN JP_MHEN
#endif
#ifndef KC_HENK
#    define KC_HENK JP_HENK
#endif

// Provided by keymap.c
void tap_code16_lang(uint16_t keycode);

// Leader internals from leader.c
extern uint16_t leader_sequence[5];
extern uint8_t  leader_sequence_size;

static bool leader_active = false;

// Ignore layer-tap thumbs and regular modifiers so users can hold LOWER/RAISE
// (KC_MHEN/KC_HENK) or Shift/Alt etc. while entering the numeric prefix.
static bool leader_ignore_key(uint16_t kc) {
    switch (kc) {
        case KC_MHEN:
        case KC_HENK:
        case KC_LSFT:
        case KC_RSFT:
        case KC_LCTL:
        case KC_RCTL:
        case KC_LALT:
        case KC_RALT:
        case KC_LGUI:
        case KC_RGUI:
            return true;
        default:
            return false;
    }
}

static bool leader_digit_for_keycode(uint16_t kc, uint8_t *digit_out) {
    switch (kc) {
        case KC_1:
        case KC_EXLM:
        case KC_KP_1:
            *digit_out = 1;
            return true;
        case KC_2:
        case KC_AT:
        case KC_KP_2:
            *digit_out = 2;
            return true;
        case KC_3:
        case KC_HASH:
        case KC_KP_3:
            *digit_out = 3;
            return true;
        case KC_4:
        case KC_DLR:
        case KC_KP_4:
            *digit_out = 4;
            return true;
        case KC_5:
        case KC_PERC:
        case KC_KP_5:
            *digit_out = 5;
            return true;
        case KC_6:
        case KC_CIRC:
        case KC_KP_6:
            *digit_out = 6;
            return true;
        case KC_7:
        case KC_AMPR:
        case KC_KP_7:
            *digit_out = 7;
            return true;
        case KC_8:
        case KC_ASTR:
        case KC_KP_8:
            *digit_out = 8;
            return true;
        case KC_9:
        case KC_LPRN:
        case KC_KP_9:
            *digit_out = 9;
            return true;
        case KC_0:
        case KC_RPRN:
        case KC_KP_0:
            *digit_out = 0;
            return true;
        default:
            return false;
    }
}

static uint8_t leader_compact_sequence(uint16_t *out) {
    uint8_t sz = 0;
    for (uint8_t i = 0; i < leader_sequence_size; i++) {
        if (leader_sequence[i] == KC_NO) {
            continue;
        }
        if (leader_ignore_key(leader_sequence[i])) {
            continue;
        }
        out[sz++] = leader_sequence[i];
    }
    return sz;
}

static uint16_t leader_parse_count(const uint16_t *seq, uint8_t seq_size, uint8_t *index_out) {
    uint16_t count = 0;
    uint8_t  idx   = 0;

    while (idx < seq_size) {
        uint8_t digit = 0;
        if (!leader_digit_for_keycode(seq[idx], &digit)) {
            break;
        }

        count = (count * 10) + digit;
        idx++;
    }

    if (count == 0) {
        count = 1;
    }

    *index_out = idx;
    return count;
}

static bool leader_match_sequence(const uint16_t *seq, uint8_t seq_size, uint8_t start_idx, uint8_t len, const uint16_t *pattern) {
    if (seq_size - start_idx != len) {
        return false;
    }
    for (uint8_t i = 0; i < len; i++) {
        if (seq[start_idx + i] != pattern[i]) {
            return false;
        }
    }
    return true;
}

static void vim_copy_lines(uint16_t count) {
    tap_code16(KC_HOME);
    register_code(KC_LSFT);
    tap_code16(KC_END);
    for (uint16_t i = 1; i < count; i++) {
        tap_code16(KC_DOWN);
        tap_code16(KC_END);
    }
    unregister_code(KC_LSFT);
    tap_code16(LCTL(KC_C));
    tap_code16(KC_HOME);  // collapse selection at line end to avoid lingering highlight
}

static void vim_paste_block_below(uint16_t lines) {
    tap_code16(KC_HOME);
    for (uint16_t i = 1; i < lines; i++) {
        tap_code16(KC_DOWN);
    }
    tap_code16(KC_END);
    tap_code16(KC_ENTER);
    tap_code16(LCTL(KC_V));
    tap_code16(KC_UP);
    tap_code16(KC_END);
    tap_code16(KC_DEL);
    tap_code16(KC_HOME);
}

static void vim_delete_lines(uint16_t count) {
    tap_code16(KC_HOME);
    register_code(KC_LSFT);
    tap_code16(KC_END);
    for (uint16_t i = 1; i < count; i++) {
        tap_code16(KC_DOWN);
    }
    unregister_code(KC_LSFT);
    tap_code16(LCTL(KC_X));
}


static void vim_undo(void) { tap_code16(LCTL(KC_Z)); }

static void vim_redo(void) { tap_code16(LCTL(KC_Y)); }

static void vim_go_top(void) { tap_code16(LCTL(KC_HOME)); }

static void vim_go_bottom(void) { tap_code16(LCTL(KC_END)); }

static void leader_led_active(void) {
    xiao_status_service_set(XIAO_STATUS_SLOT_LEADER, (xiao_rgb_t){80, 50, 0}, false);
}

static void leader_led_restore(void) {
    xiao_status_service_clear(XIAO_STATUS_SLOT_LEADER);
}

bool leader_is_active(void) { return leader_active; }

void leader_start_user(void) {
    leader_active = true;
    leader_led_active();
}

void leader_timeout_user(void) {
    leader_active = false;
    leader_led_restore();
}

void leader_end_user(void) {
    uint16_t seq[ARRAY_SIZE(leader_sequence)] = {0};
    uint8_t  seq_size                         = leader_compact_sequence(seq);

    uint8_t  idx   = 0;
    uint16_t count = leader_parse_count(seq, seq_size, &idx);

    static const uint16_t seq_yy[]  = {KC_Y, KC_Y};
    static const uint16_t seq_yyp[] = {KC_Y, KC_Y, KC_P};
    static const uint16_t seq_dd[]  = {KC_D, KC_D};

    uint8_t remaining = seq_size - idx;

    if (leader_match_sequence(seq, seq_size, idx, 2, seq_yy)) {
        vim_copy_lines(count);
    } else if (leader_match_sequence(seq, seq_size, idx, 3, seq_yyp)) {
        vim_copy_lines(count);
        vim_paste_block_below(count);
    } else if (leader_match_sequence(seq, seq_size, idx, 2, seq_dd)) {
        vim_delete_lines(count);
    } else if (remaining == 1 && seq[idx] == KC_D) {
        vim_delete_lines(count);
    } else if (remaining == 1 && seq[idx] == KC_Y) {
        vim_copy_lines(count);
    } else if (remaining == 1 && seq[idx] == KC_P) {
        for (uint16_t i = 0; i < count; i++) {
            vim_paste_block_below(1);
        }
    } else if (idx == 0 && leader_sequence_two_keys(KC_G, KC_G)) {
        vim_go_top();
    } else if (idx == 0 && leader_sequence_one_key(KC_G)) {
        vim_go_bottom();
    } else if (idx == 0 && leader_sequence_one_key(KC_U)) {
        vim_undo();
    } else if (idx == 0 && leader_sequence_one_key(KC_R)) {
        vim_redo();
    }

    leader_active = false;
    leader_led_restore();
}

bool leader_add_user(uint16_t keycode) {
    (void)keycode;

    if (leader_sequence_size == 0) {
        return false;
    }

    // Normalize the most recent key that was just added.
    uint16_t *kc = &leader_sequence[leader_sequence_size - 1];

    if (leader_ignore_key(*kc)) {
        *kc = KC_NO;
        return false;
    }

    uint8_t digit = 0;
    if (leader_digit_for_keycode(*kc, &digit)) {
        // Store as plain KC_0–KC_9 so downstream parsing is straightforward.
        *kc = (digit == 0) ? KC_0 : (KC_1 + (digit - 1));
    }

    return false;
}
