// Legacy Pico bootloader jump shim
// Copyright 2025
// SPDX-License-Identifier: GPL-2.0-or-later

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/watchdog.h"

void bootloader_jump(void) {
    reset_usb_boot(0, 0);
}

__attribute__((weak)) void mcu_reset(void) {
    watchdog_reboot(0, 0, 0);
    while (1) {
        tight_loop_contents();
    }
}