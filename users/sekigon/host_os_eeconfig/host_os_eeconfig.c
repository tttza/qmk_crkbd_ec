// Copyright 2022 sekigon-gonnoc
// SPDX-License-Identifier: GPL-2.0-or-later

#include "host_os_eeconfig.h"
#include "usb_host_os_identifier.h"
#include "action_layer.h"
#include "debug.h"
#include "keymap.h"
#include "eeprom.h"
#include "eeconfig.h"
#include "via.h"

#if RGBLIGHT_ENABLE
#    include "rgblight.h"
extern rgblight_config_t rgblight_config;
#elif RGB_MATRIX_ENABLE
#    include "rgb_matrix.h"
#endif

#ifndef EECONFIG_USER_SIZE
#    define EECONFIG_USER_SIZE EECONFIG_SIZE
#endif

#ifdef VIA_ENABLE
_Static_assert(EECONFIG_USER_SIZE*(OS_TYPE_MAX + 1) < VIA_EEPROM_MAGIC_ADDR,
               "Set VIA_EEPROM_MAGIC_ADDR larger");
#endif

__attribute__((weak)) void on_host_os_eeconfig_update(void) {}

static OS_TYPE current_os = OS_TYPE_UNKNOWN;

static int os_type_to_offset_idx(OS_TYPE os) {
    switch (os) {
        case OS_TYPE_UNKNOWN:
        case OS_TYPE_WIN:
        case OS_TYPE_MAC:
        case OS_TYPE_LINUX:
            return (EECONFIG_USER_SIZE * (os + 1));
        default:
            return 0;
    }
}

// copy current config to OS region
static void mirror_os_eeconfig(OS_TYPE os) {
    uint8_t current_config[EECONFIG_USER_SIZE];
    int     offset = os_type_to_offset_idx(os);
    eeprom_read_block(current_config, 0, EECONFIG_USER_SIZE);
    eeprom_update_block(current_config, (void*)offset, EECONFIG_USER_SIZE);
}

static void load_os_eeconfig(OS_TYPE os) {
    uint8_t os_config[EECONFIG_USER_SIZE];
    int     offset = os_type_to_offset_idx(os);
    eeprom_read_block(os_config, (void*)offset, EECONFIG_USER_SIZE);
    eeprom_update_block(os_config, 0, EECONFIG_USER_SIZE);
}

void host_os_eeconfig_init(void) {
    for (int os = 0; os < OS_TYPE_MAX; os++) {
        mirror_os_eeconfig(os);
    }
}

void host_os_eeconfig_update(OS_TYPE os) {
    if (current_os == os) {
        mirror_os_eeconfig(os);
        return;
    }

    // os changed
    current_os = os;
    load_os_eeconfig(os);

    debug_config.raw            = eeconfig_read_debug();
    keymap_config.raw           = eeconfig_read_keymap();
    layer_state_t default_layer = (layer_state_t)eeconfig_read_default_layer();
    default_layer_set(default_layer);

    layer_state_set_kb((layer_state_t)layer_state);

#if RGBLIGHT_ENABLE
    rgblight_config.raw = eeconfig_read_rgblight();
#elif RGB_MATRIX_ENABLE
    eeprom_read_block(&rgb_matrix_config, EECONFIG_RGB_MATRIX,
                      sizeof(rgb_matrix_config));
#endif

    on_host_os_eeconfig_update();
}
