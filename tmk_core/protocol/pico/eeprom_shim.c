// Minimal EEPROM bridge for Pico/TinyUSB builds
#include <stdint.h>
#include "eeprom_driver.h"

uint16_t EEPROM_Init(void) {
    eeprom_driver_init();
    return 0;
}

void EEPROM_Erase(void) {
    eeprom_driver_erase();
}
