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

#include "advanced_demo.h"
#include "pointing_device.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"

#include "pio_usb.h"


static usb_device_t *usb_device = NULL;
static uint8_t       connection_idx = 0;

void pico_before_flash_operation(void) { pio_usb_stop(); }
void pico_after_flash_operation(void) { pio_usb_restart(); }

void __not_in_flash_func(core1_main)(void) {
  sleep_ms(10);

  static pio_usb_configuration_t config = PIO_USB_DEFAULT_CONFIG;
  config.alarm_pool = (void*)alarm_pool_create(2, 8);
  config.debug_pin_rx = 5;
  config.debug_pin_eop = 7;
  usb_device = pio_usb_init(&config);


  while (true) {
    pio_usb_task();
  }
}

void keyboard_post_init_kb(void) {
    debug_enable   = true;
    debug_keyboard = true;

    multicore_reset_core1();
    multicore_launch_core1(core1_main);

    keyboard_post_init_user();
}

void matrix_scan_user(void) {
    if (usb_device != NULL && usb_device->enumerated) {
        endpoint_t *ep = &usb_device->endpoint[3];

        if ((ep->ep_num & EP_IN) && ep->new_data_flag) {
            uint8_t received[64], len;
            len = ep->packet_len;
            memcpy(received, ep->buffer, len);
            ep->new_data_flag = false;

            printf("%04x:%04x EP 0x%02x:\t", usb_device->vid, usb_device->pid,
                   ep->ep_num);
            for (size_t i = 0; i < ep->packet_len; i++) {
                printf("%02x ", received[i]);
            }
            printf("\n");

            report_mouse_t report;
            report_mouse_t report_prev = pointing_device_get_report();
            report.buttons = received[0];
            report.x = received[1];
            report.y = received[2];
            report.v = received[3];
            report.h = received[4];

            pointing_device_set_report(report);

            if (connection_idx == 0 && report_prev.buttons == 0 &&
                report.buttons) {
                SEND_STRING("with mouse!!!!");
            }
        }
    }
}

#include "via.h"

void raw_hid_receive_kb(uint8_t *data, uint8_t length) {
    if (data[0] == id_set_keyboard_value && data[1] == 0x04) {
        connection_idx = data[2] + 1;

        data[2] = matrix_get_row(0);
        data[3] = matrix_get_row(1);
        raw_hid_send(data, length);
    }
}

bool read_slave(matrix_row_t slave_matrix[], uint8_t len) {
    if (usb_device == NULL || !usb_device->enumerated) {
        memset(slave_matrix, 0, len);
        return false;
    }

    endpoint_t *ep = &usb_device->endpoint[1];

    if (!ep->new_data_flag) {
        static uint8_t raw_send[64] = {id_set_keyboard_value, 0x04};
        raw_send[2] = connection_idx;
        pio_usb_set_out_data(ep, raw_send, sizeof(raw_send));
    }

    ep = &usb_device->endpoint[2];

    static matrix_row_t slave_buffer[MATRIX_ROWS];
    if (ep->new_data_flag) {
        slave_buffer[0] = ep->buffer[2] & 0xf;
        slave_buffer[1] = ep->buffer[3] & 0xf;
        ep->new_data_flag = false;
    }
    slave_matrix[0] = slave_buffer[0];
    slave_matrix[1] = slave_buffer[1];

    return true;
}

const uint8_t col_pins[] = MATRIX_COL_PINS;
void          matrix_init_custom(void) {
    for (int c = 0; c < sizeof(col_pins); c++) {
        setPinInputHigh(col_pins[c]);
    }
}

bool matrix_scan_custom(matrix_row_t current_matrix[]) {
    bool matrix_has_changed = false;
    matrix_row_t read_matrix[MATRIX_ROWS];
    memcpy(read_matrix, current_matrix, sizeof(read_matrix));

    read_matrix[0] = 0;
    for (int c = 0; c < sizeof(col_pins); c++) {
        uint8_t state = readPin(col_pins[c]);
        read_matrix[0] |= state ? 0 : (1 << c);
    }

    read_slave(&read_matrix[1], MATRIX_ROWS - 1);

    matrix_has_changed =
        memcmp(current_matrix, read_matrix, sizeof(read_matrix));
    if (matrix_has_changed) {
        memcpy(current_matrix, read_matrix, sizeof(read_matrix));
    }

    return matrix_has_changed;
}

