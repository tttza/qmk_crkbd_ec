// Legacy Pico timer implementation
// Copyright 2025
// SPDX-License-Identifier: GPL-2.0-or-later

#include "timer.h"
#include "pico/stdlib.h"

void timer_init(void) {
}

inline uint16_t timer_read(void) {
    return (to_ms_since_boot(get_absolute_time()) & 0xFFFF);
}

inline uint32_t timer_read32(void) {
    return (to_ms_since_boot(get_absolute_time()) & 0xFFFFFFFF);
}

inline uint16_t timer_elapsed(uint16_t last) {
    uint32_t t;
    t = timer_read32();
    return TIMER_DIFF_16((t & 0xFFFF), last);
}

inline uint32_t timer_elapsed32(uint32_t last) {
    uint32_t t;
    t = timer_read32();
    return TIMER_DIFF_32(t, last);
}