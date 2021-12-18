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

#include "simple_split.h"

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/bootrom.h"

#include "pio_usb.h"
#include "usb_crc.h"


static usb_device_t *usb_device = NULL;

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
    if (usb_device != NULL) {
        for (int ep_idx = 0; ep_idx < 1; ep_idx++) {
            endpoint_t *ep = &usb_device->endpoint[ep_idx];

            if ((ep->ep_num & EP_IN) && ep->new_data_flag) {
                printf("%04x:%04x EP 0x%02x:\t", usb_device->vid,
                       usb_device->pid, ep->ep_num);
                for (size_t i = 0; i < ep->packet_len; i++) {
                    printf("%02x ", ep->buffer[i]);
                }
                printf("\n");
                ep->new_data_flag = false;
            }
        }
    }
}

#include "transport.h"
#include "via.h"

void transport_master_init(void) {}
void transport_slave_init(void) {}

bool transport_master(matrix_row_t master_matrix[],
                      matrix_row_t slave_matrix[]) {
    if (usb_device == NULL || !usb_device->enumerated) {
        return false;
    }

    endpoint_t *ep = &usb_device->endpoint[1];

    if (!ep->new_data_flag) {
        static uint8_t raw_send[64] = {id_get_keyboard_value, id_switch_matrix_state};
        pio_usb_set_out_data(ep, raw_send, sizeof(raw_send));
    }

    ep = &usb_device->endpoint[2];

    static matrix_row_t slave_buffer[MATRIX_ROWS];
    if (ep->new_data_flag) {
        slave_buffer[0] = ep->buffer[2] & 0xf;
        ep->new_data_flag = false;
    }
    slave_matrix[0] = slave_buffer[0];

    return true;
}

void transport_slave(matrix_row_t master_matrix[],
                     matrix_row_t slave_matrix[]) {
    return;
}
