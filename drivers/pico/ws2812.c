
#include "ws2812.h"
#include "ws2812.pio.h"

#include "pico/stdlib.h"

static PIO       pio = pio0;
static const int sm  = 0;

void ws2812_init(void) {
    uint offset = pio_add_program(pio, &ws2812_program);

    ws2812_program_init(pio, sm, offset, RGB_DI_PIN, 800000, false);
}

void ws2812_setleds(LED_TYPE *ledarray, uint16_t number_of_leds) {
    static bool s_init = false;
    if (!s_init) {
        ws2812_init();
        s_init = true;
    }

    printf("led %d %d %d\n", ledarray[0].r, ledarray[0].g, ledarray[0].b);

    for (int i = 0; i < number_of_leds; i++) {
        pio_sm_put_blocking(pio, sm,
                            (((uint32_t)ledarray[i].r) << 16) |
                                (((uint32_t)ledarray[i].g) << 24) |
                                ((uint32_t)ledarray[i].b << 8));
    }
}
