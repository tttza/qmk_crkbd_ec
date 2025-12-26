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

#include "usb_util.h"

#include "tusb.h"

bool usb_connected_state(void) {
    tud_task();
    return tud_connected();
}
void usb_disconnect(void) {
    // TinyUSB provides bus disconnect for force re-enumeration when needed.
    tud_disconnect();
}

// Detect USB VBUS presence/active connection for SPLIT_USB_DETECT master pick.
// When the board is not plugged into a host, TinyUSB reports disconnected, so
// the half will become the slave.
bool usb_vbus_state(void) {
    tud_task();
    return tud_connected();
}
