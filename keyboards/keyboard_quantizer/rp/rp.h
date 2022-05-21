/* Copyright 2021 sekigon-gonnoc
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

#pragma once

#include <stdint.h>
#include "keycode.h"

#define KQ_PIN_CHRST 25
#define KQ_PIN_CHBOOT 24
#define KQ_PIN_UART_TX 16
#define KQ_PIN_UART_RX 17
#define KQ_PIN_CLK 21
#define KQ_PIN_LED0 13
#define KQ_PIN_LED1 12

void uart_recv_callback(uint8_t dat);
void uart_buf_init(void);
int  send_reset_cmd(void);
int  send_led_cmd(uint8_t led);

typedef union {
    uint32_t raw;
    struct {
        uint8_t os_eeconfig : 1;
        uint8_t override_mode : 2;
        uint8_t layer_to_combo : 1;
        uint8_t tapping_term_20ms : 4;  // tapter+=20*x+60 (x>1)
        uint8_t parser_type : 1;
    };
} keyboard_config_t;
extern keyboard_config_t keyboard_config;

enum kb_keycodes {
    DISABLE_KEY_OVERRIDES = KC_FN0,
    ENABLE_US_KEY_ON_JP_OS_OVERRIDE,
    ENABLE_JP_KEY_ON_US_OS_OVERRIDE,
};
