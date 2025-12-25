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
