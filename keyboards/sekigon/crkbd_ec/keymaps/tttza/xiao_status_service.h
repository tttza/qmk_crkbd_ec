#pragma once

#include "xiao_status_led.h"

typedef enum {
    XIAO_STATUS_SLOT_STUCK_ALERT = 0,
    XIAO_STATUS_SLOT_LEADER,
    XIAO_STATUS_SLOT_CAPS_WORD,
    XIAO_STATUS_SLOT_USER,
    XIAO_STATUS_SLOT_COUNT,
} xiao_status_slot_t;

void xiao_status_service_init(void);
void xiao_status_service_set(xiao_status_slot_t slot, xiao_rgb_t color, bool blink);
void xiao_status_service_clear(xiao_status_slot_t slot);
void xiao_status_service_refresh(void);
