// Legacy Pico platform setup
// Copyright 2025
// SPDX-License-Identifier: GPL-2.0-or-later

#include "platform_deps.h"
#include "quantum.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"
#include "hardware/watchdog.h"
#include "hardware/structs/watchdog.h"
#include "bsp/board.h"

#ifndef PICO_SYSTEM_CLOCK_KHZ
#    define PICO_SYSTEM_CLOCK_KHZ 125000
#endif

uint32_t interrupts;

void platform_setup(void) {
    if (watchdog_caused_reboot() && watchdog_hw->scratch[0] == 0x2040dead) {
        bootloader_jump();
    }
    watchdog_enable(8000, 1);
    watchdog_hw->scratch[0] = 0x2040dead;

    board_init();

#if PICO_SYSTEM_CLOCK_KHZ != 125000
    set_sys_clock_khz(PICO_SYSTEM_CLOCK_KHZ, true);
#endif

    for (int i = 0; i < 32; i++) {
#ifdef RGB_DI_PIN
        if (i == RGB_DI_PIN) continue;
#endif
        gpio_init(i);
        setPinInputHigh(i);
    }

    adc_init();
}