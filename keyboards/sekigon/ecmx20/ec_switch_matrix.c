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

#include "ec_switch_matrix.h"

#include "quantum.h"
#include "analog.h"
#include "atomic_util.h"
#include "debug.h"

// sensing channel definitions
#define S0 0
#define S1 1
#define S2 2
#define S3 3
#define S4 4
#define S5 5
#define S6 6
#define S7 7

#if defined(PLATFORM_PICO)
#    define WAIT_DISCHARGE()
#    define WAIT_CHARGE() wait_us(4)
#    define cli() __interrupt_disable__()
#    define sei() __interrupt_enable__(NULL)
#else
#    define WAIT_DISCHARGE()
#    define WAIT_CHARGE()
#endif

// pin connections
const uint8_t row_pins[]     = MATRIX_ROW_PINS;
const uint8_t col_channels[] = MATRIX_COL_PINS;
const uint8_t mux_sel_pins[] = MUX_SEL_PINS;

_Static_assert(sizeof(mux_sel_pins) == 3, "invalid MUX_SEL_PINS");

static ecsm_config_t config;
static uint16_t      ecsm_sw_value[MATRIX_ROWS][MATRIX_COLS];

#ifdef ECS_VELOCITY_ENABLED
static int16_t velocity[MATRIX_ROWS][MATRIX_COLS];
#endif

static inline void discharge_capacitor(void) { setPinOutput(DISCHARGE_PIN); }
static inline void charge_capacitor(uint8_t row) {
    setPinInput(DISCHARGE_PIN);
    writePinHigh(row_pins[row]);
}

static inline void clear_all_row_pins(void) {
    for (int row = 0; row < sizeof(row_pins); row++) {
        writePinLow(row_pins[row]);
    }
}

static inline void init_mux_sel(void) {
    for (int idx = 0; idx < sizeof(mux_sel_pins); idx++) {
        setPinOutput(mux_sel_pins[idx]);
    }
}

static inline void select_mux(uint8_t col) {
    uint8_t ch = col_channels[col];
    writePin(mux_sel_pins[0], ch & 1);
    writePin(mux_sel_pins[1], ch & 2);
    writePin(mux_sel_pins[2], ch & 4);
}

static inline void init_row(void) {
    for (int idx = 0; idx < sizeof(row_pins); idx++) {
        setPinOutput(row_pins[idx]);
        writePinLow(row_pins[idx]);
    }
}

// Initialize pins
int ecsm_init(ecsm_config_t const* const ecsm_config) {
    // save config
    config = *ecsm_config;

    // initialize discharge pin as discharge mode
    writePinLow(DISCHARGE_PIN);
    setPinOutput(DISCHARGE_PIN);

    // set analog reference
#if !defined(PLATFORM_PICO)
    analogReference(ADC_REF_INTERNAL);
#endif

    // initialize drive lines
    init_row();

    // initialize multiplexer select pin
    init_mux_sel();

    // set discharge pin to charge mode
    setPinInput(DISCHARGE_PIN);
#if defined(PLATFORM_PICO)
    gpio_set_drive_strength(DISCHARGE_PIN, GPIO_DRIVE_STRENGTH_12MA);
#endif

    return 0;
}

void ecsm_get_config(ecsm_config_t* ecsm_config) {
    // Copy config
    *ecsm_config = config;
}

// Read key value of key (row, col)
static uint16_t ecsm_readkey_raw(uint8_t row, uint8_t col) {
    uint16_t sw_value = 0;

    discharge_capacitor();

    select_mux(col);

    clear_all_row_pins();

    WAIT_DISCHARGE();

    cli();

    charge_capacitor(row);

    WAIT_CHARGE();

    sw_value = analogReadPin(ANALOG_PORT);

    sei();

    return sw_value;
}

// Update press/release state of key at (row, col)
static bool ecsm_update_key(matrix_row_t* current_row, uint8_t col, uint16_t sw_value) {
    bool current_state = (*current_row >> col) & 1;

    // press to release
    if (current_state && sw_value < config.low_threshold) {
        *current_row &= ~(1 << col);
        return true;
    }

    // release to press
    if ((!current_state) && sw_value > config.high_threshold) {
        *current_row |= (1 << col);
        return true;
    }

    return false;
}

// Scan key values and update matrix state
bool ecsm_matrix_scan(matrix_row_t current_matrix[]) {
    bool updated = false;

#ifdef ECS_VELOCITY_ENABLED
    static uint16_t prev_time = 0;
    static bool     first     = true;

    uint16_t dt = timer_elapsed(prev_time);
    while (dt == 0) {
        dt = timer_elapsed(prev_time);
    }
#endif

    for (int col = 0; col < sizeof(col_channels); col++) {
        for (int row = 0; row < sizeof(row_pins); row++) {
#ifdef ECS_VELOCITY_ENABLED
            uint16_t current = ecsm_readkey_raw(row, col);

            if (!first) {
                int32_t current_velocity = ((int32_t)current - ecsm_sw_value[row][col]) * 100 / dt;
                const uint8_t filt = 8;
                velocity[row][col]       = (filt * (int32_t)velocity[row][col] + (10 - filt) * current_velocity) / 10;
            } else {
                first = false;
            }

            ecsm_sw_value[row][col] = current;
#else
            ecsm_sw_value[row][col] = ecsm_readkey_raw(row, col);
#endif
            updated |= ecsm_update_key(&current_matrix[row], col, ecsm_sw_value[row][col]);
        }
    }

#ifdef ECS_VELOCITY_ENABLED
    prev_time = timer_read();
#endif

    return updated;
}

// Debug print key values
void ecsm_dprint_matrix(void) {
    for (int row = 0; row < sizeof(row_pins); row++) {
        for (int col = 0; col < sizeof(col_channels); col++) {
            dprintf("%4d", ecsm_sw_value[row][col]);
            if (col < sizeof(col_channels) - 1) {
                dprintf(",");
            }
        }
        dprintf("\n");
    }
    dprintf("\n");
    // dprintf("%d,%d,%d,%d,%d\n", ecsm_sw_value[0][0], ecsm_sw_value[0][1], ecsm_sw_value[0][2], ecsm_sw_value[0][3],ecsm_sw_value[1][1]);
}

#ifdef ECS_VELOCITY_ENABLED
int16_t        ecsm_get_velocity(uint8_t row, uint8_t col) { return velocity[row][col]; }
void           ecsm_dprint_velocity(void) {
    for (int row = 0; row < sizeof(row_pins); row++) {
        for (int col = 0; col < sizeof(col_channels); col++) {
            dprintf("%4d", velocity[row][col]);
            if (col < sizeof(col_channels) - 1) {
                dprintf(",");
            }
        }
        dprintf("\n");
    }
    dprintf("\n");
}
#endif
