// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

typedef enum {
    OS_TYPE_UNKNOWN,
    OS_TYPE_WIN,
    OS_TYPE_MAC,
    OS_TYPE_LINUX,
} OS_TYPE;

OS_TYPE get_usb_host_os_type(void);

void reset_usb_host_os_identity(void);
void set_usb_host_os_identity_device_req_len(uint16_t len);
void set_usb_host_os_identity_config_req_len(uint16_t len);
