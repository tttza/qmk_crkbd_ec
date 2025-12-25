// Legacy Pico GPIO compatibility for platform/gpio.h shims
// SPDX-License-Identifier: GPL-2.0-or-later

#pragma once

#include "pico/stdlib.h"

static inline void gpio_set_pin_input(uint32_t pin) {
    gpio_disable_pulls(pin);
    gpio_set_dir(pin, GPIO_IN);
}

static inline void gpio_set_pin_input_high(uint32_t pin) {
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_up(pin);
}

static inline void gpio_set_pin_input_low(uint32_t pin) {
    gpio_set_dir(pin, GPIO_IN);
    gpio_pull_down(pin);
}

static inline void gpio_set_pin_output_push_pull(uint32_t pin) {
    gpio_set_dir(pin, GPIO_OUT);
}

static inline void gpio_set_pin_output_open_drain(uint32_t pin) {
    gpio_set_dir(pin, GPIO_OUT);
}

static inline void gpio_write_pin_high(uint32_t pin) {
    gpio_put(pin, 1);
}

static inline void gpio_write_pin_low(uint32_t pin) {
    gpio_put(pin, 0);
}

static inline void gpio_write_pin(uint32_t pin, bool level) {
    gpio_put(pin, level);
}

static inline bool gpio_read_pin(uint32_t pin) {
    return gpio_get(pin);
}

static inline void gpio_toggle_pin(uint32_t pin) {
    gpio_put(pin, !gpio_get(pin));
}