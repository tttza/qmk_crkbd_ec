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
