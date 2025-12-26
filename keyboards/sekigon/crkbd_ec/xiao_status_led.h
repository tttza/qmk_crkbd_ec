// Xiao RP2040 onboard indicator helpers
#pragma once

#include "quantum.h"
#include "led.h"
typedef struct {
	uint8_t r;
	uint8_t g;
	uint8_t b;
} xiao_rgb_t;

void xiao_status_led_init(void);
void xiao_status_led_set_layer(layer_state_t state);
void xiao_status_led_set_host_leds(led_t leds);
// Alert mask: bit0=red, bit1=green, bit2=blue
void xiao_status_led_set_alert(uint8_t mask);
// Status color overrides the layer color; blink toggles on/off automatically.
void xiao_status_led_set_status(xiao_rgb_t color, bool blink);
void xiao_status_led_task(void);
void xiao_status_led_suspend(void);
void xiao_status_led_wakeup(void);
