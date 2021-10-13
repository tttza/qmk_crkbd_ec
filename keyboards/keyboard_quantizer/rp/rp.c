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

#include "keyboard_quantizer.h"
#include "rp.h"
#include "report_descriptor_parser.h"
#include "report_parser.h"

#include <string.h>

#include "quantum.h"
#include "debug.h"

#include "pico_cdc.h"
#include "hardware/uart.h"
#include "hardware/irq.h"
#include "cdc_device.h"

#define LEN(x) (sizeof(x) / sizeof(x[0]))

extern bool    ch559_update_mode;
extern uint8_t device_cnt;
extern uint8_t hid_info_cnt;

uint8_t qt_cmd_buf[3];
bool    qt_cmd_new;

static uint8_t  buf[1024];
static uint16_t widx, ridx, cnt;

void uart_buf_init(void) {
    memset(buf, 0, sizeof(buf));
    widx = 0;
    ridx = 0;
    cnt  = 0;
}

void uart_recv_callback(uint8_t dat) {
    if (cnt < sizeof(buf) - 1) {
        buf[widx] = dat;
        widx      = (widx + 1) % sizeof(buf);
        cnt++;
    }
}

bool uart_available(void) { return cnt > 0; }

uint8_t uart_getchar(void) {
    uint8_t c = 0;
    if (cnt > 0) {
        c = buf[ridx];
        cnt--;
        ridx = (ridx + 1) % sizeof(buf);
    }

    return c;
}

void uart_putchar(uint8_t c) {
    // BMPAPI->uart.send(&c, 1);
}

int send_reset_cmd(void) {
    // send reset command to ch559

    writePinHigh(KQ_PIN_CHRST);
    xprintf("send reset\n");
    writePinLow(KQ_PIN_CHRST);

    return 0;
}

int send_led_cmd(uint8_t led) {
    if (!qt_cmd_new) {
        dprintf("send led command:%d\n", led);
        qt_cmd_buf[0] = 0x80 | led;
        qt_cmd_new    = true;

        return 0;
    } else {
        return 1;
    }
}

#define KQ_UART uart0

void on_uart_rx(void) {
    while (uart_is_readable(KQ_UART)) {
        uint8_t ch = uart_getc(KQ_UART);
        tud_cdc_write_char(ch);
    }
    tud_cdc_write_flush();
}

static void init_uart(void) {
    uart_init(KQ_UART, 57600);
    setPinInputHigh(KQ_PIN_UART_RX);
    gpio_set_function(KQ_PIN_UART_TX, GPIO_FUNC_UART);
    gpio_set_function(KQ_PIN_UART_RX, GPIO_FUNC_UART);

    uart_set_fifo_enabled(KQ_UART, false);

    uart_set_hw_flow(KQ_UART, false, false);

    uart_set_format(KQ_UART, 8, 1, UART_PARITY_NONE);

    const int UART_IRQ = KQ_UART == uart0 ? UART0_IRQ : UART1_IRQ;

    irq_set_exclusive_handler(UART_IRQ, on_uart_rx);
    irq_set_enabled(UART_IRQ, true);

    uart_set_irq_enables(KQ_UART, true, false);
}

void activate_ch55x_bootloader(void) {
    if (ch559_update_mode) {
        return;
    }
    ch559_update_mode = true;

    writePinHigh(KQ_PIN_CHRST);
    // xprintf("Assert reset\n");
    writePinHigh(KQ_PIN_CHBOOT);
    wait_ms(10);
    // xprintf("Dessert reset\n");
    writePinLow(KQ_PIN_CHRST);

    pico_cdc_disable_printf();

    wait_ms(1000);
    init_uart();
}

void pico_cdc_receive_cb(uint8_t const* buf, uint32_t cnt) {
    if (ch559_update_mode) {
        uart_write_blocking(KQ_UART, buf, cnt);
    } else if (cnt > 0) {
        printf("%c\n", buf[0]);

        switch (buf[0]) {
            case 'c':
                send_reset_cmd();
                break;
            case 'd':
                debug_enable = true;
                break;
            case 'b':
                bootloader_jump();
                break;
        }
    }
}

void pico_cdc_change_baudrate_cb(uint32_t baudrate) {
    if (baudrate == 1200) {
        bootloader_jump();
    } else if (baudrate == 57600) {
        activate_ch55x_bootloader();
        writePinHigh(KQ_PIN_LED0);
    }
}

void keyboard_post_init_kb_rev(void) { debug_enable = false; }

void dynamic_keymap_reset() {
    for (int idx = 0; idx < DYNAMIC_KEYMAP_LAYER_COUNT * MATRIX_COLS_DEFAULT *
                                MATRIX_ROWS_DEFAULT;
         idx++) {
        const uint8_t layer = idx / (MATRIX_COLS_DEFAULT * MATRIX_ROWS_DEFAULT);
        const uint8_t offset =
            idx % (MATRIX_COLS_DEFAULT * MATRIX_ROWS_DEFAULT);
        const uint8_t row = offset / MATRIX_COLS_DEFAULT;
        const uint8_t col = offset % MATRIX_COLS_DEFAULT;

        if (row < MATRIX_MODIFIER_ROW) {
            dynamic_keymap_set_keycode(
                layer, row, col,
                idx % (MATRIX_COLS_DEFAULT * MATRIX_ROWS_DEFAULT));
        } else if (row == MATRIX_MODIFIER_ROW) {
            dynamic_keymap_set_keycode(layer, row, col, KC_LCTRL + col);
        } else if (row == MATRIX_MSBTN_ROW) {
            dynamic_keymap_set_keycode(layer, row, col, KC_BTN1 + col);
        } else if (row == MATRIX_MSGES_ROW) {
            if (col < MATRIX_MSWHEEL_COL) {
                dynamic_keymap_set_keycode(layer, row, col, KC_NO);
            } else {
                dynamic_keymap_set_keycode(
                    layer, row, col, KC_MS_WH_UP + col - MATRIX_MSWHEEL_COL);
            }
        }
    }
}

