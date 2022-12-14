// -------------------------------------------------- //
// This file is autogenerated by pioasm; do not edit! //
// -------------------------------------------------- //

#pragma once

#if !PICO_NO_HARDWARE
#include "hardware/pio.h"
#endif

#define IRQ_RX_BS_ERR 1
#define IRQ_RX_EOP 2

// ------ //
// usb_rx //
// ------ //

#define usb_rx_wrap_target 0
#define usb_rx_wrap 23

static const uint16_t usb_rx_program_instructions[] = {
            //     .wrap_target
    0xe045, //  0: set    y, 5                       
    0x2020, //  1: wait   0 pin, 0                   
    0x00c7, //  2: jmp    pin, 7                     
    0xe120, //  3: set    x, 0                   [1] 
    0x4121, //  4: in     x, 1                   [1] 
    0xe045, //  5: set    y, 5                       
    0x010d, //  6: jmp    13                     [1] 
    0xe221, //  7: set    x, 1                   [2] 
    0x4021, //  8: in     x, 1                       
    0x0282, //  9: jmp    y--, 2                 [2] 
    0xe045, // 10: set    y, 5                       
    0x00d7, // 11: jmp    pin, 23                    
    0x050d, // 12: jmp    13                     [5] 
    0x00d3, // 13: jmp    pin, 19                    
    0xe321, // 14: set    x, 1                   [3] 
    0x4121, // 15: in     x, 1                   [1] 
    0x008d, // 16: jmp    y--, 13                    
    0xe045, // 17: set    y, 5                       
    0x0602, // 18: jmp    2                      [6] 
    0xe320, // 19: set    x, 0                   [3] 
    0x4021, // 20: in     x, 1                       
    0xe045, // 21: set    y, 5                       
    0x0002, // 22: jmp    2                          
    0xc021, // 23: irq    wait 1                     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program usb_rx_program = {
    .instructions = usb_rx_program_instructions,
    .length = 24,
    .origin = -1,
};

static inline pio_sm_config usb_rx_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + usb_rx_wrap_target, offset + usb_rx_wrap);
    return c;
}
#endif

// ------------ //
// usb_rx_debug //
// ------------ //

#define usb_rx_debug_wrap_target 0
#define usb_rx_debug_wrap 23

static const uint16_t usb_rx_debug_program_instructions[] = {
            //     .wrap_target
    0xe045, //  0: set    y, 5            side 0     
    0x2020, //  1: wait   0 pin, 0        side 0     
    0x10c7, //  2: jmp    pin, 7          side 1     
    0xe120, //  3: set    x, 0            side 0 [1] 
    0x5121, //  4: in     x, 1            side 1 [1] 
    0xf045, //  5: set    y, 5            side 1     
    0x110d, //  6: jmp    13              side 1 [1] 
    0xe221, //  7: set    x, 1            side 0 [2] 
    0x5021, //  8: in     x, 1            side 1     
    0x1282, //  9: jmp    y--, 2          side 1 [2] 
    0xf045, // 10: set    y, 5            side 1     
    0x10d7, // 11: jmp    pin, 23         side 1     
    0x150d, // 12: jmp    13              side 1 [5] 
    0x10d3, // 13: jmp    pin, 19         side 1     
    0xe321, // 14: set    x, 1            side 0 [3] 
    0x5121, // 15: in     x, 1            side 1 [1] 
    0x108d, // 16: jmp    y--, 13         side 1     
    0xf045, // 17: set    y, 5            side 1     
    0x1602, // 18: jmp    2               side 1 [6] 
    0xe320, // 19: set    x, 0            side 0 [3] 
    0x5021, // 20: in     x, 1            side 1     
    0xf045, // 21: set    y, 5            side 1     
    0x1002, // 22: jmp    2               side 1     
    0xc021, // 23: irq    wait 1          side 0     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program usb_rx_debug_program = {
    .instructions = usb_rx_debug_program_instructions,
    .length = 24,
    .origin = -1,
};

static inline pio_sm_config usb_rx_debug_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + usb_rx_debug_wrap_target, offset + usb_rx_debug_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}
#endif

// ---------- //
// eop_detect //
// ---------- //

#define eop_detect_wrap_target 0
#define eop_detect_wrap 6

static const uint16_t eop_detect_program_instructions[] = {
            //     .wrap_target
    0xe041, //  0: set    y, 1                       
    0xa0c3, //  1: mov    isr, null                  
    0x4002, //  2: in     pins, 2                    
    0xa026, //  3: mov    x, isr                     
    0x0040, //  4: jmp    x--, 0                     
    0x0082, //  5: jmp    y--, 2                     
    0xc022, //  6: irq    wait 2                     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program eop_detect_program = {
    .instructions = eop_detect_program_instructions,
    .length = 7,
    .origin = -1,
};

static inline pio_sm_config eop_detect_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + eop_detect_wrap_target, offset + eop_detect_wrap);
    return c;
}
#endif

// ---------------- //
// eop_detect_debug //
// ---------------- //

#define eop_detect_debug_wrap_target 0
#define eop_detect_debug_wrap 6

static const uint16_t eop_detect_debug_program_instructions[] = {
            //     .wrap_target
    0xe041, //  0: set    y, 1            side 0     
    0xa0c3, //  1: mov    isr, null       side 0     
    0x4002, //  2: in     pins, 2         side 0     
    0xa026, //  3: mov    x, isr          side 0     
    0x0040, //  4: jmp    x--, 0          side 0     
    0x0082, //  5: jmp    y--, 2          side 0     
    0xd022, //  6: irq    wait 2          side 1     
            //     .wrap
};

#if !PICO_NO_HARDWARE
static const struct pio_program eop_detect_debug_program = {
    .instructions = eop_detect_debug_program_instructions,
    .length = 7,
    .origin = -1,
};

static inline pio_sm_config eop_detect_debug_program_get_default_config(uint offset) {
    pio_sm_config c = pio_get_default_sm_config();
    sm_config_set_wrap(&c, offset + eop_detect_debug_wrap_target, offset + eop_detect_debug_wrap);
    sm_config_set_sideset(&c, 1, false, false);
    return c;
}

#include "hardware/clocks.h"
static inline void usb_rx_program_init(PIO pio, uint sm, uint offset, uint pin_dp, int pin_debug) {
    pio_sm_set_consecutive_pindirs(pio, sm, pin_dp, 2, false);
    gpio_pull_down(pin_dp);
    gpio_pull_down(pin_dp + 1);  // dm
    pio_sm_config c;
    if (pin_debug < 0) {
      c = usb_rx_program_get_default_config(offset);
    } else {
      c = usb_rx_debug_program_get_default_config(offset);
      pio_sm_set_pins_with_mask(pio, sm, 0, 1 << pin_debug);
      pio_sm_set_pindirs_with_mask(pio, sm, 1 << pin_debug, 1 << pin_debug);
      pio_gpio_init(pio, pin_debug);
      sm_config_set_sideset_pins(&c, pin_debug);
    }
    sm_config_set_in_pins(&c, pin_dp);  // for WAIT, IN
    sm_config_set_jmp_pin(&c, pin_dp);  // for JMP
    // Shift to right, autopull enabled, 8bit
    sm_config_set_in_shift(&c, true, true, 8);
    sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_RX);
    // Run at 96Mhz
    // system clock should be multiple of 12MHz
    float div = (float)clock_get_hz(clk_sys) / (96000000);
    sm_config_set_clkdiv(&c, div);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, false);
}
static inline void eop_detect_program_init(PIO pio, uint sm, uint offset,
                                           uint pin_dp, int pin_debug) {
  pio_sm_config c;
  if (pin_debug < 0) {
    c = eop_detect_program_get_default_config(offset);
  } else {
    c = eop_detect_debug_program_get_default_config(offset);
    pio_sm_set_pins_with_mask(pio, sm, 0, 1 << pin_debug);
    pio_sm_set_pindirs_with_mask(pio, sm, 1 << pin_debug, 1 << pin_debug);
    pio_gpio_init(pio, pin_debug);
    sm_config_set_sideset_pins(&c, pin_debug);
  }
  sm_config_set_in_pins(&c, pin_dp);  // for WAIT, IN
  sm_config_set_jmp_pin(&c, pin_dp);  // for JMP
  sm_config_set_in_shift(&c, false, false, 8);
  float div = (float)clock_get_hz(clk_sys) / (96000000);
  sm_config_set_clkdiv(&c, div);
  pio_sm_init(pio, sm, offset, &c);
  pio_sm_set_enabled(pio, sm, true);
}

#endif

