// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include QMK_KEYBOARD_H

#include "ec_switch_matrix.h"
#include "matrix.h"
#include "debug.h"
#include "eeprom.h"
#include "split_util.h"
#include "transport.h"
#include "debounce.h"

#ifndef LOW_THRESHOLD
#    define LOW_THRESHOLD 200
#endif

#ifndef HIGH_THRESHOLD
#    define HIGH_THRESHOLD 300
#endif

#define ROWS_PER_HAND (MATRIX_ROWS / 2)

/* matrix state(1:on, 0:off) */
extern matrix_row_t raw_matrix[MATRIX_ROWS];  // raw values
extern matrix_row_t matrix[MATRIX_ROWS];      // debounced values

// row offsets for each hand
uint8_t thisHand, thatHand;

// user-defined overridable functions
__attribute__((weak)) void matrix_slave_scan_user(void) {}
__attribute__((weak)) void matrix_slave_scan_kb(void) { matrix_scan_user(); }

void matrix_init_custom(void) {
    split_pre_init();

    ecsm_config_t ecsm_config;
#ifdef EEPROM_ECS_THRESHOLD_ADDR
    ecsm_config.low_threshold =
        eeprom_read_word((uint16_t*)EEPROM_ECS_THRESHOLD_ADDR);
    ecsm_config.high_threshold =
        eeprom_read_word((uint16_t*)(EEPROM_ECS_THRESHOLD_ADDR + 2));
#else
    ecsm_config.low_threshold  = LOW_THRESHOLD;
    ecsm_config.high_threshold = HIGH_THRESHOLD;
#endif

    ecsm_init(&ecsm_config);

    thisHand = isLeftHand ? 0 : (ROWS_PER_HAND);
    thatHand = ROWS_PER_HAND - thisHand;

    split_post_init();
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool updated = ecsm_matrix_scan(current_matrix);

    return updated;
}

__attribute__((weak)) bool transport_master_if_connected(
    matrix_row_t master_matrix[], matrix_row_t slave_matrix[]) {
    transport_master(master_matrix, slave_matrix);
    return true;
}

bool matrix_post_scan(void) {
    bool changed = false;
    if (is_keyboard_master()) {
        static bool  last_connected              = false;
        matrix_row_t slave_matrix[ROWS_PER_HAND] = {0};
        if (transport_master_if_connected(matrix + thisHand, slave_matrix)) {
            changed = memcmp(matrix + thatHand, slave_matrix,
                             sizeof(slave_matrix)) != 0;

            last_connected = true;
        } else if (last_connected) {
            // reset other half when disconnected
            memset(slave_matrix, 0, sizeof(slave_matrix));
            changed = true;

            last_connected = false;
        }

        if (changed)
            memcpy(matrix + thatHand, slave_matrix, sizeof(slave_matrix));

        matrix_scan_quantum();
    } else {
        transport_slave(matrix + thatHand, matrix + thisHand);

        matrix_slave_scan_kb();
    }

    return changed;
}

uint8_t matrix_scan(void) {
    bool changed = matrix_scan_custom(raw_matrix) || matrix_post_scan();

    debounce(raw_matrix, matrix + thisHand, ROWS_PER_HAND, changed);

    return changed;
}
