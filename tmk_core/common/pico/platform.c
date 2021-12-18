
#include "platform_deps.h"
#include "quantum.h"
#include "hardware/adc.h"
#include "hardware/clocks.h"

#ifndef PICO_SYSTEM_CLOCK_KHZ
#    define PICO_SYSTEM_CLOCK_KHZ 125000
#endif

uint32_t interrupts;

void platform_setup(void) {

    set_sys_clock_khz(PICO_SYSTEM_CLOCK_KHZ, true);

    for (int i = 0; i < 32; i++) {
        gpio_init(i);
        setPinInputHigh(i);
    }

    adc_init();
}
