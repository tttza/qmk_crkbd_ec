
#include "platform_deps.h"
#include "quantum.h"

uint32_t interrupts;

void platform_setup(void) {
    for (int i = 0; i < 32; i++) {
        setPinInputHigh(i);
    }
}
