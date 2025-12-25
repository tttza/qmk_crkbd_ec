// Pico PIO-backed WS2812 driver hook for custom mode
#include "ws2812.h"
#include "platforms/pico/_pin_defs.h"
#include "../pico/ws2812.pio.h"

#include "atomic_util.h"
#include "pio_manager.h"
#include "boards/pico_boards.h"

#include "pico/stdlib.h"

#ifndef WS2812_RES
#    define WS2812_RES (1000 * WS2812_TRST_US)
#endif

static PIO pio = pio0;
static int sm  = -1;
LED_TYPE ws2812_leds[WS2812_LED_COUNT];

static int ws2812_try_init(void) {
	sm = pio_manager_get_empty_sm(pio);

	if (sm < 0) {
		return -1;
	}

	int32_t offset = pio_manager_add_program(pio, sm, &ws2812_program);

	if (offset < 0) {
		return -1;
	}

	ws2812_program_init(pio, sm, offset, WS2812_DI_PIN, 800000, false);

	return 0;
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
	if (sm < 0 && ws2812_try_init() != 0) {
		return;
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
