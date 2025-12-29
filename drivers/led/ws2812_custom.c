// Pico PIO-backed WS2812 driver hook for custom mode
#include "ws2812.h"
#include "platforms/pico/_pin_defs.h"
#include "../pico/ws2812.pio.h"

#include "atomic_util.h"
#include "pio_manager.h"
#include "boards/pico_boards.h"

#include "hardware/pio.h"
#include "hardware/structs/pio.h"
#include "pico/stdlib.h"

#ifndef LED_TYPE
#    define LED_TYPE ws2812_led_t
#endif

#ifndef WS2812_RES
#    define WS2812_RES (1000 * WS2812_TRST_US)
#endif

static PIO pio = pio0;
static int sm  = -1;
LED_TYPE ws2812_leds[WS2812_LED_COUNT];

static inline bool ws2812_sm_enabled(PIO target_pio, int sm_index) {
	return (target_pio->ctrl & (1u << (sm_index + PIO_CTRL_SM_ENABLE_LSB))) != 0;
}

static int ws2812_program_load(PIO target_pio, int candidate_sm) {
    int32_t offset = pio_manager_add_program(target_pio, candidate_sm, &ws2812_program);

    if (offset < 0) {
        pio_sm_unclaim(target_pio, candidate_sm);
        return -1;
    }

	pio = target_pio;
	sm  = candidate_sm;
	ws2812_program_init(pio, sm, offset, WS2812_DI_PIN, 800000, false);
    return 0;
}

static int ws2812_try_init(void) {
	int candidate = pio_manager_get_empty_sm(pio0);
	if (candidate >= 0 && ws2812_program_load(pio0, candidate) == 0) {
		return 0;
	}

	// Fall back to PIO1 if PIO0 is busy (e.g. soft serial already claimed SMs)
	candidate = pio_manager_get_empty_sm(pio1);
	if (candidate >= 0 && ws2812_program_load(pio1, candidate) == 0) {
		return 0;
	}

	return -1;
}

void ws2812_init(void) {
	if (sm >= 0) {
		return;
	}

	ws2812_try_init();
}

void ws2812_set_color(int index, uint8_t red, uint8_t green, uint8_t blue) {
	if (index < 0 || index >= WS2812_LED_COUNT) {
		return;
	}

	ws2812_leds[index].r = red;
	ws2812_leds[index].g = green;
	ws2812_leds[index].b = blue;
}

void ws2812_set_color_all(uint8_t red, uint8_t green, uint8_t blue) {
	for (int i = 0; i < WS2812_LED_COUNT; i++) {
		ws2812_set_color(i, red, green, blue);
	}
}

void ws2812_setleds(LED_TYPE *ledarray, uint16_t number_of_leds) {
	// Re-init if SM was never acquired or got disabled (e.g. after bus resets).
	if (sm < 0 || !ws2812_sm_enabled(pio, sm)) {
		sm = -1;
		if (ws2812_try_init() != 0) {
			return;
		}
	}

	__interrupt_disable__();

	for (uint16_t i = 0; i < number_of_leds; i++) {
		pio_sm_put_blocking(pio, sm, (((uint32_t)ledarray[i].r) << 16) | (((uint32_t)ledarray[i].g) << 24) | ((uint32_t)ledarray[i].b << 8));
	}

	__interrupt_enable__(NULL);

	while (!pio_sm_is_tx_fifo_empty(pio, sm)) {
		continue;
	}
	busy_wait_us(WS2812_RES / 1000);
}

void ws2812_flush(void) {
	ws2812_setleds(ws2812_leds, WS2812_LED_COUNT);
}
