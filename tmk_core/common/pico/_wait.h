
#pragma once

#include "pico/stdlib.h"

#define wait_ms(ms) busy_wait_ms(ms)
#define wait_us(us) busy_wait_us(us)
#define waitInputPinDelay() wait_cpuclock(GPIO_INPUT_PIN_DELAY)

#include "_wait.c"
