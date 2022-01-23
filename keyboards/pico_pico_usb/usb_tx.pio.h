// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

// ------ //
// usb_tx //
// ------ //

#define usb_tx_wrap_target 2
#define usb_tx_wrap 21

#define usb_tx_IRQ_COMP 0

static const uint16_t usb_tx_program_instructions[] = {
    0xf445, //  0: set    y, 5            side 1     
    0xe083, //  1: set    pindirs, 3                 
            //     .wrap_target
    0x00ea, //  2: jmp    !osre, 10                  
    0xa142, //  3: nop                           [1] 
    0xb342, //  4: nop                    side 0 [3] 
    0xa342, //  5: nop                           [3] 
    0xb442, //  6: nop                    side 1     
    0xe080, //  7: set    pindirs, 0                 
    0xc020, //  8: irq    wait 0                     
    0x0000, //  9: jmp    0                          
    0x6021, // 10: out    x, 1                       
    0x002e, // 11: jmp    !x, 14                     
    0x1482, // 12: jmp    y--, 2          side 1     
    0xa242, // 13: nop                           [2] 
    0xf845, // 14: set    y, 5            side 2     
    0x00f1, // 15: jmp    !osre, 17                  
    0x0104, // 16: jmp    4                      [1] 
    0x6021, // 17: out    x, 1                       
    0x0035, // 18: jmp    !x, 21                     
    0x188f, // 19: jmp    y--, 15         side 2     
    0xa242, // 20: nop                           [2] 
    0xf445, // 21: set    y, 5            side 1     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program usb_tx_program = {
    .instructions = usb_tx_program_instructions,
    .length = 22,
    .origin = -1,
};

static inline pio_sm_config usb_tx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + usb_tx_wrap_target, offset + usb_tx_wrap);
    sm_config_set_sideset(&c, 3, true, false);
    return c;
}

#include "hardware/clocks.h"
  static inline void usb_tx_program_init(PIO pio, uint sm, uint offset,
                                         uint pin_dp) {
    pio_sm_set_pins_with_mask(pio, sm, (0b01 << pin_dp), (0b11 << pin_dp));
    gpio_pull_down(pin_dp);
    gpio_pull_down(pin_dp + 1); // dm
    pio_gpio_init(pio, pin_dp);
    pio_gpio_init(pio, pin_dp + 1); // dm
    pio_sm_config c = usb_tx_program_get_default_config(offset);
    // shifts to left, autopull, 8bit
    sm_config_set_out_shift(&c, true, true, 8);
    sm_config_set_out_pins(&c, pin_dp, 2);
    sm_config_set_set_pins(&c, pin_dp, 2);
    sm_config_set_sideset_pins(&c, pin_dp);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX);
    // run at 48MHz
    // clk_sys should be multiply of 12MHz
    float div = (float)clock_get_hz(clk_sys) / (48000000UL);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);
  }

#endif
