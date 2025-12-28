#include QMK_KEYBOARD_H
#include "leader.h"
#include "process_keycode/process_leader.h"
#include "xiao_status_service.h"

// Provided by keymap.c
void tap_code16_lang(uint16_t keycode);

// Leader internals from leader.c
extern uint16_t leader_sequence[5];
extern uint8_t  leader_sequence_size;

static uint16_t leader_parse_count(uint8_t *index_out) {
    uint16_t count = 0;
    uint8_t  idx   = 0;

    while (idx < leader_sequence_size) {
        uint16_t kc = leader_sequence[idx];
        if (kc >= KC_1 && kc <= KC_0) {  // digits 1-0
            uint8_t digit = (kc == KC_0) ? 0 : (kc - KC_1 + 1);
            count         = (count * 10) + digit;
            idx++;
        } else {
            break;
        }
    }

    if (count == 0) {
        count = 1;
    }

    *index_out = idx;
    return count;
}

static bool leader_match_sequence(uint8_t start_idx, uint8_t len, const uint16_t *seq) {
    if (leader_sequence_size - start_idx != len) {
        return false;
    }
    for (uint8_t i = 0; i < len; i++) {
        if (leader_sequence[start_idx + i] != seq[i]) {
            return false;
        }
    }
    return true;
}

static void tap_seq(const uint16_t *seq, uint8_t len) {
    for (uint8_t i = 0; i < len; i++) {
        tap_code16(seq[i]);
    }
}

static void vim_copy_lines(uint16_t count) {
    tap_code16(KC_HOME);
    register_code(KC_LSFT);
    tap_code16(KC_END);
    for (uint16_t i = 1; i < count; i++) {
        tap_code16(KC_DOWN);
    }
    unregister_code(KC_LSFT);
    tap_code16(LCTL(KC_C));
    tap_code16(KC_END);  // collapse selection at line end to avoid lingering highlight
}

static void vim_paste_below(void) {
    tap_code16(KC_END);
    tap_code16(KC_ENT);
    tap_code16(LCTL(KC_V));
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

static void vim_paste(void) {
    tap_code16(KC_END);
    tap_code16(KC_ENT);
    tap_code16(LCTL(KC_V));
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

void leader_start_user(void) { leader_led_active(); }

void leader_timeout_user(void) { leader_led_restore(); }

void leader_end_user(void) {
    uint8_t  idx   = 0;
    uint16_t count = leader_parse_count(&idx);

    static const uint16_t seq_yy[]  = {KC_Y, KC_Y};
    static const uint16_t seq_yyp[] = {KC_Y, KC_Y, KC_P};
    static const uint16_t seq_dd[]  = {KC_D, KC_D};

    uint8_t remaining = leader_sequence_size - idx;

    if (leader_match_sequence(idx, 2, seq_yy)) {
        vim_copy_lines(count);
    } else if (leader_match_sequence(idx, 3, seq_yyp)) {
        vim_copy_lines(count);
        vim_paste_below();
    } else if (leader_match_sequence(idx, 2, seq_dd)) {
        vim_delete_lines(count);
    } else if (remaining == 1 && leader_sequence[idx] == KC_D) {
        vim_delete_lines(count);
    } else if (remaining == 1 && leader_sequence[idx] == KC_Y) {
        vim_copy_lines(count);
    } else if (remaining == 1 && leader_sequence[idx] == KC_P) {
        for (uint16_t i = 0; i < count; i++) {
            vim_paste();
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

    leader_led_restore();
}
