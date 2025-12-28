#include QMK_KEYBOARD_H

#include "ec_switch_matrix.h"
#include "protocol/pico/pico_cdc.h"
#include "debug.h"

static bool dprint_matrix = false;

void pico_cdc_on_disconnect(void) {
    dprint_matrix = false;
    debug_matrix  = false;
    debug_enable  = false;
}

bool pico_cdc_receive_kb(uint8_t const *buf, uint32_t cnt) {
    if (cnt > 0 && buf[0] == 'e') {
        dprint_matrix ^= true;
        if (dprint_matrix) {
            debug_enable = true;
            debug_matrix = true;
        } else {
            debug_matrix = false;
            debug_enable = false;
        }
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
