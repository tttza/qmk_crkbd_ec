// Enable ADC for RP2040 generic board
// Generated to support EC analog matrix on crkbd_ec
#pragma once

#define HAL_USE_ADC  TRUE
#define PAL_USE_WAIT TRUE

#include_next <halconf.h>
