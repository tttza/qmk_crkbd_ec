// Enable ADC for EC sensing on RP2040
#pragma once

#include_next <halconf.h>

#undef HAL_USE_ADC
#define HAL_USE_ADC TRUE

#undef PAL_USE_WAIT
#define PAL_USE_WAIT TRUE

#undef PAL_USE_CALLBACKS
#define PAL_USE_CALLBACKS TRUE
