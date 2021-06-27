
#include "pico/stdlib.h"

#define FLASH_SIZE (FLASHEND + 1L)

#if !defined(BOOTLOADER_SIZE)
uint16_t bootloader_start;
#endif

__attribute__((weak)) void bootloader_jump(void) {
#ifdef PICO_BOOTSEL_VIA_DOUBLE_RESET_ACTIVITY_LED
    const uint32_t led_mask = 1u << PICO_BOOTSEL_VIA_DOUBLE_RESET_ACTIVITY_LED;
#else
    const uint32_t led_mask = 0u;
#endif
    reset_usb_boot(led_mask, 0);
}
