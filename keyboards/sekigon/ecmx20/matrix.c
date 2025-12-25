/* Copyright 2020 sekigon-gonnoc
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
#include "matrix.h"
#include "debug.h"
#include "eeprom.h"

#ifndef LOW_THRESHOLD
#    define LOW_THRESHOLD 200
#endif

#ifndef HIGH_THRESHOLD
#    define HIGH_THRESHOLD 300
#endif

#define ENCODER_PUSH_BTN B6

void matrix_init_custom(void) {
    ecsm_config_t ecsm_config;
#ifdef EEPROM_ECS_THRESHOLD_ADDR
    ecsm_config.low_threshold  = eeprom_read_word((uint16_t*)EEPROM_ECS_THRESHOLD_ADDR);
    ecsm_config.high_threshold = eeprom_read_word((uint16_t*)(EEPROM_ECS_THRESHOLD_ADDR + 2));
#else
    ecsm_config.low_threshold  = LOW_THRESHOLD;
    ecsm_config.high_threshold = HIGH_THRESHOLD;
#endif

    ecsm_init(&ecsm_config);

    setPinInputHigh(ENCODER_PUSH_BTN);
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool updated = ecsm_matrix_scan(current_matrix);

    if (!readPin(ENCODER_PUSH_BTN)) {
        if (!(current_matrix[5] & (1 << 0))) {
            updated |= true;
            current_matrix[5] |= (1 << 0);
        }
    } else {
        if ((current_matrix[5] & (1 << 0))) {
            updated |= true;
            current_matrix[5] &= ~(1 << 0);
        }
    }

    return updated;
}

