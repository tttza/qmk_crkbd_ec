// Legacy Pico bootloader jump shim
// Copyright 2025
// SPDX-License-Identifier: GPL-2.0-or-later

#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "hardware/watchdog.h"

// Avoid forcing BOOTSEL unless explicitly requested; reboot like a normal reset
// so we can tell if firmware is accidentally calling the bootloader path.
void bootloader_jump(void) {
    watchdog_reboot(0, 0, 0);
    while (1) {
        tight_loop_contents();
    }
}

__attribute__((weak)) void mcu_reset(void) {
    watchdog_reboot(0, 0, 0);
    while (1) {
        tight_loop_contents();
    }
}