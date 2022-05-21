// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later
#pragma once

#include <stdint.h>

typedef union {
    uint32_t raw;
    struct {
        uint8_t gesture_threshold : 4;
    };
} user_config_t;
extern user_config_t user_config;
