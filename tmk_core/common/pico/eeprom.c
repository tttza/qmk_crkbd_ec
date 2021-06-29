
#include <string.h>

// qmk
#include "eeprom.h"
#include "eeconfig.h"

// pico
#include "pico/stdlib.h"
#include "hardware/flash.h"
#include "hardware/sync.h"

// TODO Define from Flash size
#define EEPEMU_EECONFIG_START_OFFSET 0x40000  // First 256kB is program area
#define EEPEMU_KEYMAP_START_OFFSET 0x41000

#define EEPEMU_EECONFIG_SIZE 1024
#define EEPEMU_KEYMAP_SIZE 4096

_Static_assert(EEPEMU_EECONFIG_START_OFFSET % FLASH_SECTOR_SIZE == 0,
               "Offset should be aligned to FLASH_SCTOR_SIZE");
_Static_assert(EEPEMU_KEYMAP_START_OFFSET % FLASH_SECTOR_SIZE == 0,
               "Offset should be aligned to FLASH_SCTOR_SIZE");

static uint8_t eepemu_eeconfig[EEPEMU_EECONFIG_SIZE];      // 1kB
static uint8_t eepemu_dynamic_keymap[EEPEMU_KEYMAP_SIZE];  // 4kB

// Load ROM data to RAM
void pico_eepemu_init(void) {
    memcpy(eepemu_eeconfig, (void *)XIP_BASE + EEPEMU_EECONFIG_START_OFFSET,
           sizeof(eepemu_eeconfig));
    memcpy(eepemu_dynamic_keymap, (void *)XIP_BASE + EEPEMU_KEYMAP_START_OFFSET,
           sizeof(eepemu_dynamic_keymap));
}

void pico_eepemu_flash_eeconfig(void) {
    int32_t status = save_and_disable_interrupts();

    flash_range_erase(EEPEMU_EECONFIG_START_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(EEPEMU_EECONFIG_START_OFFSET, eepemu_eeconfig,
                        sizeof(eepemu_eeconfig));

    restore_interrupts(status);
}

void pico_eepemu_flash_dynamic_keymap(void) {
    int32_t status = save_and_disable_interrupts();

    flash_range_erase(EEPEMU_KEYMAP_START_OFFSET, FLASH_SECTOR_SIZE);
    flash_range_program(EEPEMU_KEYMAP_START_OFFSET, eepemu_dynamic_keymap,
                        sizeof(eepemu_dynamic_keymap));

    restore_interrupts(status);
}

#ifndef DYNAMIC_KEYMAP_EEPROM_ADDR
#    ifdef VIA_EEPROM_CUSTOM_CONFIG_ADDR
#        define DYNAMIC_KEYMAP_EEPROM_ADDR \
            (VIA_EEPROM_CUSTOM_CONFIG_ADDR + VIA_EEPROM_CUSTOM_CONFIG_SIZE)
#    else
#        define DYNAMIC_KEYMAP_EEPROM_ADDR (EECONFIG_SIZE + 1)
#    endif
#endif

static void *convert_address(const void *addr) {
    // TODO Assert address
    if ((uint32_t)addr < DYNAMIC_KEYMAP_EEPROM_ADDR) {
        return (void *)(eepemu_eeconfig + (uint32_t)addr);
    } else {
        return (void *)(eepemu_dynamic_keymap + (uint32_t)addr -
                        DYNAMIC_KEYMAP_EEPROM_ADDR);
    }
}

uint8_t eeprom_read_byte(const uint8_t *__p) {
    uint8_t *addr = convert_address(__p);
    if (addr == NULL) {
        return 0xff;
    }

    return *addr;
}

uint16_t eeprom_read_word(const uint16_t *__p) {
    uint16_t *addr = convert_address(__p);
    if (addr == NULL) {
        return 0xffff;
    }

    if ((uint32_t)addr % 2 == 0) {
        return *addr;
    } else {
        uint16_t ret;
        memcpy(&ret, addr, sizeof(ret));
        return ret;
    }
}

uint32_t eeprom_read_dword(const uint32_t *__p) {
    uint32_t *addr = convert_address(__p);
    if (addr == NULL) {
        return 0xffffffff;
    }

    if ((uint32_t)addr % 4 == 0) {
        return *addr;
    } else {
        uint32_t ret;
        memcpy(&ret, addr, sizeof(ret));
        return ret;
    }
}

void eeprom_read_block(void *__dst, const void *__src, size_t __n) {
    uint32_t *addr = convert_address(__src);
    if (addr == NULL) {
        return;
    }

    memcpy(__dst, addr, __n);
}

void eeprom_write_byte(uint8_t *__p, uint8_t __value) {
    uint8_t *addr = convert_address(__p);
    if (addr == NULL) {
        return;
    }
    *addr = __value;
}

void eeprom_write_word(uint16_t *__p, uint16_t __value) {
    uint16_t *addr = convert_address(__p);
    if (addr == NULL) {
        return;
    }

    if ((uint32_t)addr % 2 == 0) {
        *addr = __value;
    } else {
        memcpy(addr, &__value, sizeof(__value));
    }
}
void eeprom_write_dword(uint32_t *__p, uint32_t __value) {
    uint32_t *addr = convert_address(__p);
    if (addr == NULL) {
        return;
    }

    if ((uint32_t)addr % 4 == 0) {
        *addr = __value;
    } else {
        memcpy(addr, &__value, sizeof(__value));
    }
}

void eeprom_write_block(const void *__src, void *__dst, size_t __n) {
    uint32_t *addr = convert_address(__dst);
    if (addr == NULL) {
        return;
    }

    memcpy(addr, __src, __n);
}

void eeprom_update_byte(uint8_t *__p, uint8_t __value) {
    eeprom_write_byte(__p, __value);
}

void eeprom_update_word(uint16_t *__p, uint16_t __value) {
    eeprom_write_word(__p, __value);
}

void eeprom_update_dword(uint32_t *__p, uint32_t __value) {
    eeprom_write_dword(__p, __value);
}

void eeprom_update_block(const void *__src, void *__dst, size_t __n) {
    eeprom_write_block(__src, __dst, __n);
}
