#include "xiao_status_service.h"

static const xiao_status_slot_t priority_order[] = {
    XIAO_STATUS_SLOT_STUCK_ALERT,
    XIAO_STATUS_SLOT_LEADER,
    XIAO_STATUS_SLOT_CAPS_WORD,
    XIAO_STATUS_SLOT_USER,
};

typedef char xiao_status_priority_size_check[(XIAO_STATUS_SLOT_COUNT == 4) ? 1 : -1];

typedef struct {
    bool       active;
    bool       blink;
    xiao_rgb_t color;
} xiao_status_slot_state_t;

static xiao_status_slot_state_t slots[XIAO_STATUS_SLOT_COUNT];
static xiao_status_slot_t       applied_slot  = XIAO_STATUS_SLOT_COUNT;
static xiao_rgb_t               applied_color = {0, 0, 0};
static bool                     applied_blink = false;

static bool colors_equal(xiao_rgb_t a, xiao_rgb_t b) {
    return a.r == b.r && a.g == b.g && a.b == b.b;
}

static void xiao_status_service_apply(void) {
    xiao_rgb_t         color  = (xiao_rgb_t){0, 0, 0};
    bool               blink  = false;
    xiao_status_slot_t chosen = XIAO_STATUS_SLOT_COUNT;

    for (uint8_t i = 0; i < sizeof(priority_order) / sizeof(priority_order[0]); i++) {
        xiao_status_slot_t slot = priority_order[i];
        if (slots[slot].active) {
            color  = slots[slot].color;
            blink  = slots[slot].blink;
            chosen = slot;
            break;
        }
    }

    if (chosen == applied_slot && colors_equal(color, applied_color) && blink == applied_blink) {
        return;
    }

    applied_slot  = chosen;
    applied_color = color;
    applied_blink = blink;

    xiao_status_led_set_status(color, blink);
}

void xiao_status_service_init(void) {
    for (uint8_t i = 0; i < XIAO_STATUS_SLOT_COUNT; i++) {
        slots[i].active = false;
        slots[i].blink  = false;
        slots[i].color  = (xiao_rgb_t){0, 0, 0};
    }

    applied_slot  = XIAO_STATUS_SLOT_COUNT;
    applied_color = (xiao_rgb_t){0, 0, 0};
    applied_blink = false;

    xiao_status_service_apply();
}

void xiao_status_service_set(xiao_status_slot_t slot, xiao_rgb_t color, bool blink) {
    if (slot >= XIAO_STATUS_SLOT_COUNT) {
        return;
    }

    slots[slot].active = true;
    slots[slot].color  = color;
    slots[slot].blink  = blink;

    xiao_status_service_apply();
}

void xiao_status_service_clear(xiao_status_slot_t slot) {
    if (slot >= XIAO_STATUS_SLOT_COUNT) {
        return;
    }

    slots[slot].active = false;
    slots[slot].blink  = false;
    slots[slot].color  = (xiao_rgb_t){0, 0, 0};

    xiao_status_service_apply();
}

void xiao_status_service_refresh(void) { xiao_status_service_apply(); }
