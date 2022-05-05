// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include <stdint.h>
#include "usb_host_os_identifier.h"

static int16_t os_identity_usb_first_device_req_len = -1;
static int16_t os_identity_usb_first_config_req_len = -1;

OS_TYPE get_usb_host_os_type(void) {
    if (os_identity_usb_first_config_req_len == 0xff) {
        return OS_TYPE_WIN;
    } else if (os_identity_usb_first_device_req_len == 8) {
        return OS_TYPE_MAC;

    } else if (os_identity_usb_first_device_req_len == 18) {
        return OS_TYPE_LINUX;
    }

    return OS_TYPE_UNKNOWN;
}

void reset_usb_host_os_identity(void) {
    os_identity_usb_first_device_req_len = -1;
    os_identity_usb_first_config_req_len = -1;
}

void set_usb_host_os_identity_device_req_len(uint16_t len) {
    if (os_identity_usb_first_device_req_len == -1) {
        os_identity_usb_first_device_req_len = len;
    }
}

void set_usb_host_os_identity_config_req_len(uint16_t len) {
    if (os_identity_usb_first_config_req_len == -1) {
        os_identity_usb_first_config_req_len = len;
    }
}
