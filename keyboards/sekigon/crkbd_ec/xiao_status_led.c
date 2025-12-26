// Drive the Xiao RP2040 onboard WS2812 (GP12, power on GP11) and discrete user LEDs (GP16/17/25)
#include "xiao_status_led.h"

#include "protocol/pico/pio_manager.h"
#include "drivers/pico/ws2812.pio.h"
#include "ws2812.h"

#include "hardware/pio.h"
#include "hardware/structs/pio.h"
#include "pico/stdlib.h"
#include "timer.h"

#ifndef XIAO_STATUS_LED_ACTIVE_HIGH
#    define XIAO_STATUS_LED_ACTIVE_HIGH 1
#endif

#define XIAO_STATUS_BLINK_MS 500  // blink period for alerts

#define XIAO_LAYER_COLOR_COUNT 4

static const xiao_rgb_t layer_palette[XIAO_LAYER_COLOR_COUNT] = {
    {0, 0, 0},    // _QWERTY (off)
    {72, 32, 0},  // _LOWER
    {80, 0, 30},  // _RAISE (magenta-leaning)
    {0, 72, 88},  // _ADJUST / fallback (bright teal)
};

static PIO     pixel_pio          = NULL;
static int     pixel_sm           = -1;
static bool    pixel_ready        = false;
static bool    pixel_powered      = false;
static xiao_rgb_t pixel_color     = {0, 0, 0};
static led_t   last_host_leds     = {0};
static layer_state_t last_layer_state = 0;
static xiao_rgb_t status_color    = {0, 0, 0};
static bool      status_blink     = false;
static bool      blink_on         = true;
static uint32_t  blink_timer_ms   = 0;
static xiao_rgb_t layer_color     = {0, 0, 0};

static inline void write_user_led(uint8_t pin, bool on) {
    writePin(pin, XIAO_STATUS_LED_ACTIVE_HIGH ? on : !on);
}

static inline bool pixel_sm_is_enabled(PIO target, int sm_index) {
    return (target->ctrl & (1u << (sm_index + PIO_CTRL_SM_ENABLE_LSB))) != 0;
}

static void xiao_user_leds_apply(led_t state) {
    setPinOutput(XIAO_STATUS_LED_R_PIN);
    setPinOutput(XIAO_STATUS_LED_G_PIN);
    setPinOutput(XIAO_STATUS_LED_B_PIN);

    write_user_led(XIAO_STATUS_LED_R_PIN, state.caps_lock);
    write_user_led(XIAO_STATUS_LED_G_PIN, state.num_lock);
    write_user_led(XIAO_STATUS_LED_B_PIN, state.scroll_lock);
}

static void xiao_user_leds_off(void) {
    led_t off = {0};
    xiao_user_leds_apply(off);
}

static void xiao_pixel_power(bool enable) {
    setPinOutput(XIAO_STATUS_PIXEL_POWER_PIN);
    writePin(XIAO_STATUS_PIXEL_POWER_PIN, enable);
    pixel_powered = enable;
    if (!enable) {
        pixel_ready = false;
        pixel_sm    = -1;
    }
}

static bool xiao_pixel_try_init(PIO target) {
    int candidate_sm = pio_manager_get_empty_sm(target);
    if (candidate_sm < 0) {
        return false;
    }

    int32_t offset = pio_manager_add_program(target, candidate_sm, &ws2812_program);
    if (offset < 0) {
        pio_sm_unclaim(target, candidate_sm);
        return false;
    }

    pixel_pio   = target;
    pixel_sm    = candidate_sm;
    pixel_ready = true;

    ws2812_program_init(pixel_pio, pixel_sm, offset, XIAO_STATUS_PIXEL_DI_PIN, 800000, false);
    return true;
}

static bool xiao_pixel_ensure_ready(void) {
    if (pixel_ready && pixel_sm >= 0 && pixel_sm_is_enabled(pixel_pio, pixel_sm)) {
        return true;
    }

    pixel_ready = false;
    pixel_sm    = -1;

    if (!pixel_powered) {
        xiao_pixel_power(true);
    }

    // Prefer PIO0 so soft-serial can keep PIO1 exclusively (Xiao RP2040 split link).
    if (xiao_pixel_try_init(pio0)) {
        return true;
    }

    if (xiao_pixel_try_init(pio1)) {
        return true;
    }

    pixel_ready = false;
    pixel_sm    = -1;
    return false;
}

static bool color_is_zero(xiao_rgb_t c) {
    return c.r == 0 && c.g == 0 && c.b == 0;
}

static void xiao_pixel_send(xiao_rgb_t color);

static void xiao_status_led_apply(void) {
    xiao_rgb_t target = layer_color;

    if (!color_is_zero(status_color)) {
        target = (status_blink && !blink_on) ? (xiao_rgb_t){0, 0, 0} : status_color;
    }

    xiao_pixel_send(target);
}

static void xiao_pixel_send(xiao_rgb_t color) {
    if (!xiao_pixel_ensure_ready()) {
        return;
    }

    if (pixel_color.r == color.r && pixel_color.g == color.g && pixel_color.b == color.b) {
        return;
    }

    pixel_color = color;

    uint32_t packed = ((uint32_t)color.g << 24) | ((uint32_t)color.r << 16) | ((uint32_t)color.b << 8);

    pio_sm_put_blocking(pixel_pio, pixel_sm, packed);
    while (!pio_sm_is_tx_fifo_empty(pixel_pio, pixel_sm)) {
        tight_loop_contents();
    }
    sleep_us(WS2812_TRST_US);
}

void xiao_status_led_init(void) {
    xiao_pixel_power(true);
    xiao_user_leds_apply(last_host_leds);
    xiao_status_led_set_layer(last_layer_state);
}

void xiao_status_led_set_layer(layer_state_t state) {
    last_layer_state = state;

    uint8_t layer = get_highest_layer(state);
    xiao_rgb_t  color = {0, 0, 0};
    if (layer < XIAO_LAYER_COLOR_COUNT) {
        color = layer_palette[layer];
    }

    layer_color = color;
    xiao_status_led_apply();
}

void xiao_status_led_set_host_leds(led_t leds) {
    last_host_leds = leds;
}

void xiao_status_led_set_alert(uint8_t mask) {
    led_t state = {0};
    state.caps_lock   = (mask & 0x01);  // red
    state.num_lock    = (mask & 0x02);  // green
    state.scroll_lock = (mask & 0x04);  // blue

    xiao_user_leds_apply(state);
}

void xiao_status_led_set_status(xiao_rgb_t color, bool blink) {
    bool next_blink = blink && !color_is_zero(color);

    if (status_color.r == color.r && status_color.g == color.g && status_color.b == color.b && status_blink == next_blink) {
        // No change; avoid resetting blink timer.
        return;
    }

    status_color   = color;
    status_blink   = next_blink;
    blink_on       = true;
    blink_timer_ms = timer_read32();

    xiao_status_led_apply();
}

void xiao_status_led_task(void) {
    if (!status_blink || color_is_zero(status_color)) {
        return;
    }

    uint32_t elapsed = timer_elapsed32(blink_timer_ms);
    if (elapsed < XIAO_STATUS_BLINK_MS) {
        return;
    }

    // Catch up in case of long delays.
    blink_timer_ms += elapsed;
    blink_on = !blink_on;
    xiao_status_led_apply();
}

void xiao_status_led_suspend(void) {
    xiao_pixel_send((xiao_rgb_t){0, 0, 0});
    xiao_user_leds_off();
    xiao_pixel_power(false);
}

void xiao_status_led_wakeup(void) {
    xiao_pixel_power(true);
    xiao_user_leds_apply((led_t){0});
    xiao_status_led_set_status(status_color, status_blink);
}
