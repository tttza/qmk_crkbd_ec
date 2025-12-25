// Legacy Pico pin definitions for data-driven platform glue
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include <stdint.h>

typedef uint32_t pin_t;

#define GP0 0
#define GP1 1
#define GP2 2
#define GP3 3
#define GP4 4
#define GP5 5
#define GP6 6
#define GP7 7
#define GP8 8
#define GP9 9
#define GP10 10
#define GP11 11
#define GP12 12
#define GP13 13
#define GP14 14
#define GP15 15
#define GP16 16
#define GP17 17
#define GP18 18
#define GP19 19
#define GP20 20
#define GP21 21
#define GP22 22
#define GP23 23
#define GP24 24
#define GP25 25
#define GP26 26
#define GP27 27
#define GP28 28
#define GP29 29

#define IS_VALID_PIN(p) ((p) != NO_PIN)