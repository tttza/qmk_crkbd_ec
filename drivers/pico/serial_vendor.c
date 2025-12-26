// PIO-based split serial driver for RP2040 (vendor-style) on the pico platform.
// This is a minimal, polling-based port of the ChibiOS vendor PIO driver
// tailored to QMK's soft serial API.

#include "quantum.h"
#include "serial.h"
#include "wait.h"

#include "debug.h"

#ifndef SERIAL_DEBUG_LOG
#    define SERIAL_DEBUG_LOG 0
#endif
#if SERIAL_DEBUG_LOG
#    define SDPRINTLN(msg) dprintln(msg)
#    define SDPRINTF(...) dprintf(__VA_ARGS__)
#else
#    define SDPRINTLN(msg)
#    define SDPRINTF(...)
#endif

#include "pico/stdlib.h"
#include "hardware/pio.h"
#include "hardware/clocks.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"
#include "hardware/sync.h"

// -----------------------------------------------------------------------------
// Configuration
// -----------------------------------------------------------------------------
#ifndef SELECT_SOFT_SERIAL_SPEED
#    define SELECT_SOFT_SERIAL_SPEED 1
#endif

#ifndef SERIAL_USART_TIMEOUT
#    define SERIAL_USART_TIMEOUT 30  // ms timeout for each blocking op
#endif

#ifndef SERIAL_PIO_USE_PIO1
#    define SERIAL_PIO_USE_PIO1 0
#endif

#if SELECT_SOFT_SERIAL_SPEED == 0
#    define SERIAL_USART_SPEED 460800
#elif SELECT_SOFT_SERIAL_SPEED == 1
#    define SERIAL_USART_SPEED 230400
#elif SELECT_SOFT_SERIAL_SPEED == 2
#    define SERIAL_USART_SPEED 115200
#elif SELECT_SOFT_SERIAL_SPEED == 3
#    define SERIAL_USART_SPEED 57600
#elif SELECT_SOFT_SERIAL_SPEED == 4
#    define SERIAL_USART_SPEED 38400
#elif SELECT_SOFT_SERIAL_SPEED == 5
#    define SERIAL_USART_SPEED 19200
#else
#    error invalid SELECT_SOFT_SERIAL_SPEED value
#endif

// -----------------------------------------------------------------------------
// PIO programs (lifted from platforms/chibios/drivers/vendor/RP/RP2040/serial_vendor.c)
// -----------------------------------------------------------------------------
#define UART_TX_WRAP_TARGET 0
#define UART_TX_WRAP 3

#if defined(SERIAL_USART_FULL_DUPLEX)
static const uint16_t uart_tx_program_instructions[] = {
    0x9fa0, 0xf727, 0x6001, 0x0642,
};
#else
static const uint16_t uart_tx_program_instructions[] = {
    0x9fa0, 0xf727, 0x6081, 0x0642,
};
#endif

static const pio_program_t uart_tx_program = {
    .instructions = uart_tx_program_instructions,
    .length       = 4,
    .origin       = -1,
};

#define UART_RX_WRAP_TARGET 0
#define UART_RX_WRAP 8
static const uint16_t uart_rx_program_instructions[] = {
    0x2020, 0xea27, 0x4001, 0x0642, 0x00c8, 0xc020, 0x20a0, 0x0000, 0x8020,
};

static const pio_program_t uart_rx_program = {
    .instructions = uart_rx_program_instructions,
    .length       = 9,
    .origin       = -1,
};

// -----------------------------------------------------------------------------
// Driver state
// -----------------------------------------------------------------------------
static PIO  pio         = SERIAL_PIO_USE_PIO1 ? pio1 : pio0;
static int  tx_sm       = -1;
static int  rx_sm       = -1;
static bool in_rx_state = false;
static void enter_rx_state(void);
static void leave_rx_state(void);

// -----------------------------------------------------------------------------
// Helpers
// -----------------------------------------------------------------------------
static inline uint32_t ms_to_ticks(uint32_t ms) { return ms * 1000; }

static inline bool timeout_reached(absolute_time_t deadline) {
    return time_reached(deadline);
}

static inline void wait_fifo_empty(void) {
    while (!pio_sm_is_tx_fifo_empty(pio, tx_sm)) {
        tight_loop_contents();
    }
}

// Clear RX FIFO to drop stale bytes after errors.
static inline void serial_transport_driver_clear(void) {
    pio_sm_set_enabled(pio, rx_sm, false);
    pio_sm_clear_fifos(pio, rx_sm);
    pio_sm_restart(pio, rx_sm);
    pio_sm_set_enabled(pio, rx_sm, true);
}

static inline void recover_after_error(void) {
    in_rx_state = false;
    serial_transport_driver_clear();
    enter_rx_state();
}

// Half-duplex helper: augment weak pull-up while listening, mirroring the
// legacy vendor driver behavior. This improves rise times on long traces.
static void enter_rx_state(void) {
    if (in_rx_state) return;

    // Let any outstanding byte finish.
    wait_fifo_empty();
    // Wait ~11 bits (1 start + 8 data + 1 stop + slack) to avoid truncating.
    sleep_us(1000000U * 11U / SERIAL_USART_SPEED);

    // Disable TX SM to allow pin mode tweaks.
    pio_sm_set_enabled(pio, tx_sm, false);
    // Lower drive so the peer can still drive low, but keep the line biased high.
    gpio_set_drive_strength(SOFT_SERIAL_PIN, GPIO_DRIVE_STRENGTH_2MA);
    // Drive high via PIO pin value, then release direction to input.
    pio_sm_set_pins_with_mask(pio, tx_sm, 1u << SOFT_SERIAL_PIN, 1u << SOFT_SERIAL_PIN);
    pio_sm_set_consecutive_pindirs(pio, tx_sm, SOFT_SERIAL_PIN, 1, false);

    pio_sm_set_enabled(pio, rx_sm, true);
    in_rx_state = true;
}

static void leave_rx_state(void) {
    if (!in_rx_state) return;

    // Stop RX before we take control of the line.
    pio_sm_set_enabled(pio, rx_sm, false);
    // Take ownership and drive high strongly for TX.
    pio_sm_set_consecutive_pindirs(pio, tx_sm, SOFT_SERIAL_PIN, 1, true);
    pio_sm_set_pins_with_mask(pio, tx_sm, 1u << SOFT_SERIAL_PIN, 1u << SOFT_SERIAL_PIN);
    gpio_set_drive_strength(SOFT_SERIAL_PIN, GPIO_DRIVE_STRENGTH_12MA);
    pio_sm_restart(pio, tx_sm);
    pio_sm_set_enabled(pio, tx_sm, true);

    in_rx_state = false;
}

static bool pio_send_bytes(const uint8_t* data, size_t size, uint32_t timeout_ms) {
    leave_rx_state();
    absolute_time_t deadline = make_timeout_time_us(ms_to_ticks(timeout_ms));
    for (size_t i = 0; i < size; ++i) {
        while (pio_sm_is_tx_fifo_full(pio, tx_sm)) {
            if (timeout_reached(deadline)) {
                recover_after_error();
                return false;
            }
            tight_loop_contents();
        }
        pio_sm_put(pio, tx_sm, data[i]);
    }
    wait_fifo_empty();
    enter_rx_state();
    return true;
}

static bool pio_recv_bytes(uint8_t* data, size_t size, uint32_t timeout_ms) {
    enter_rx_state();
    absolute_time_t deadline = make_timeout_time_us(ms_to_ticks(timeout_ms));
    for (size_t i = 0; i < size; ++i) {
        while (pio_sm_is_rx_fifo_empty(pio, rx_sm)) {
            if (timeout_reached(deadline)) {
                recover_after_error();
                return false;
            }
            tight_loop_contents();
        }
        uint32_t raw = pio_sm_get(pio, rx_sm);
        data[i]      = (uint8_t)raw;
    }
    return true;
}

static bool pio_recv_bytes_blocking(uint8_t* data, size_t size) {
    enter_rx_state();
    for (size_t i = 0; i < size; ++i) {
        while (pio_sm_is_rx_fifo_empty(pio, rx_sm)) {
            tight_loop_contents();
        }
        uint32_t raw = pio_sm_get(pio, rx_sm);
        data[i]      = (uint8_t)raw;
    }
    return true;
}

// -----------------------------------------------------------------------------
// PIO init
// -----------------------------------------------------------------------------
static void pio_tx_init(pin_t tx_pin) {
    uint offset = pio_add_program(pio, &uart_tx_program);

    tx_sm = pio_claim_unused_sm(pio, true);
    pio_sm_config config = pio_get_default_sm_config();
    sm_config_set_wrap(&config, offset + UART_TX_WRAP_TARGET, offset + UART_TX_WRAP);
#if defined(SERIAL_USART_FULL_DUPLEX)
    sm_config_set_sideset(&config, 2, true, false);
#else
    sm_config_set_sideset(&config, 2, true, true);
#endif
    sm_config_set_out_shift(&config, true, false, 32);
    sm_config_set_out_pins(&config, tx_pin, 1);
    sm_config_set_sideset_pins(&config, tx_pin);
    sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_TX);
    float div = (float)clock_get_hz(clk_sys) / (8 * SERIAL_USART_SPEED);
    sm_config_set_clkdiv(&config, div);

    pio_sm_set_consecutive_pindirs(pio, tx_sm, tx_pin, 1, true);
    pio_sm_set_pins_with_mask(pio, tx_sm, 1u << tx_pin, 1u << tx_pin);
    pio_sm_init(pio, tx_sm, offset, &config);
    pio_sm_set_enabled(pio, tx_sm, true);
}

static void pio_rx_init(pin_t rx_pin) {
    uint offset = pio_add_program(pio, &uart_rx_program);

    rx_sm = pio_claim_unused_sm(pio, true);
    pio_sm_config config = pio_get_default_sm_config();
    sm_config_set_wrap(&config, offset + UART_RX_WRAP_TARGET, offset + UART_RX_WRAP);
    sm_config_set_in_pins(&config, rx_pin);
    sm_config_set_jmp_pin(&config, rx_pin);
    sm_config_set_in_shift(&config, true, false, 32);
    sm_config_set_fifo_join(&config, PIO_FIFO_JOIN_RX);
    float div = (float)clock_get_hz(clk_sys) / (8 * SERIAL_USART_SPEED);
    sm_config_set_clkdiv(&config, div);

    pio_sm_set_consecutive_pindirs(pio, rx_sm, rx_pin, 1, false);
    pio_sm_init(pio, rx_sm, offset, &config);
    pio_sm_set_enabled(pio, rx_sm, true);
}

static void pio_serial_init(void) {
    // Ensure line idles high.
    gpio_put(SOFT_SERIAL_PIN, 1);
    gpio_set_dir(SOFT_SERIAL_PIN, true);
    gpio_set_function(SOFT_SERIAL_PIN, GPIO_FUNC_PIO0 + (SERIAL_PIO_USE_PIO1 ? 1 : 0));
    gpio_set_drive_strength(SOFT_SERIAL_PIN, GPIO_DRIVE_STRENGTH_12MA);
    gpio_set_slew_rate(SOFT_SERIAL_PIN, GPIO_SLEW_RATE_FAST);
    gpio_pull_up(SOFT_SERIAL_PIN);

    pio_tx_init(SOFT_SERIAL_PIN);
    pio_rx_init(SOFT_SERIAL_PIN);

    pio_sm_clear_fifos(pio, tx_sm);
    pio_sm_clear_fifos(pio, rx_sm);

    enter_rx_state();
}

// -----------------------------------------------------------------------------
// Transaction helpers (serial_protocol style)
// -----------------------------------------------------------------------------
static inline bool send_token(uint8_t token) {
    return pio_send_bytes(&token, 1, SERIAL_USART_TIMEOUT);
}

static inline bool recv_token(uint8_t* token, bool blocking) {
    if (blocking) {
        return pio_recv_bytes_blocking(token, 1);
    }
    return pio_recv_bytes(token, 1, SERIAL_USART_TIMEOUT);
}

static inline bool react_to_transaction(void) {
    uint8_t transaction_id = 0;
    if (!recv_token(&transaction_id, true)) {
        recover_after_error();
        return false;
    }

    if (transaction_id >= NUM_TOTAL_TRANSACTIONS) {
        recover_after_error();
        return false;
    }

    split_transaction_desc_t* transaction = &split_transaction_table[transaction_id];

    uint8_t handshake = transaction_id ^ NUM_TOTAL_TRANSACTIONS;
    if (!send_token(handshake)) {
        recover_after_error();
        return false;
    }

    if (transaction->initiator2target_buffer_size) {
        if (!pio_recv_bytes(split_trans_initiator2target_buffer(transaction), transaction->initiator2target_buffer_size, SERIAL_USART_TIMEOUT)) {
            recover_after_error();
            return false;
        }
    }

    if (transaction->slave_callback) {
        transaction->slave_callback(transaction->initiator2target_buffer_size, split_trans_initiator2target_buffer(transaction), transaction->target2initiator_buffer_size, split_trans_target2initiator_buffer(transaction));
    }

    if (transaction->target2initiator_buffer_size) {
        if (!pio_send_bytes(split_trans_target2initiator_buffer(transaction), transaction->target2initiator_buffer_size, SERIAL_USART_TIMEOUT)) {
            recover_after_error();
            return false;
        }
    }

    return true;
}

// GPIO edge IRQ to wake the slave path on the first start bit.
static void __no_inline_not_in_flash_func(gpio_irq_handler)(uint gpio, uint32_t events) {
    if (gpio != SOFT_SERIAL_PIN || !(events & GPIO_IRQ_EDGE_FALL)) {
        return;
    }
    uint32_t irq_state = save_and_disable_interrupts();
    gpio_set_irq_enabled_with_callback(SOFT_SERIAL_PIN, GPIO_IRQ_EDGE_FALL, false, gpio_irq_handler);
    (void)react_to_transaction();
    gpio_set_irq_enabled_with_callback(SOFT_SERIAL_PIN, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);
    restore_interrupts(irq_state);
}

// -----------------------------------------------------------------------------
// Public API
// -----------------------------------------------------------------------------
void soft_serial_initiator_init(void) {
    pio_serial_init();
    SDPRINTLN("vendor PIO initiator");
}

void soft_serial_target_init(void) {
    pio_serial_init();
    gpio_set_irq_enabled_with_callback(SOFT_SERIAL_PIN, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);
    irq_set_priority(IO_IRQ_BANK0, PICO_HIGHEST_IRQ_PRIORITY);
    SDPRINTLN("vendor PIO target");
}

bool soft_serial_transaction(int index) {
    if (index >= NUM_TOTAL_TRANSACTIONS) {
        return false;
    }

    split_transaction_desc_t* transaction = &split_transaction_table[index];

    if (!send_token((uint8_t)index)) {
        SDPRINTLN("tx handshake fail");
        recover_after_error();
        return false;
    }

    uint8_t handshake = 0;
    if (!recv_token(&handshake, false) || handshake != ((uint8_t)index ^ NUM_TOTAL_TRANSACTIONS)) {
        SDPRINTLN("rx handshake fail");
        recover_after_error();
        return false;
    }

    if (transaction->initiator2target_buffer_size) {
        if (!pio_send_bytes(split_trans_initiator2target_buffer(transaction), transaction->initiator2target_buffer_size, SERIAL_USART_TIMEOUT)) {
            SDPRINTLN("tx payload fail");
            recover_after_error();
            return false;
        }
    }

    if (transaction->target2initiator_buffer_size) {
        if (!pio_recv_bytes(split_trans_target2initiator_buffer(transaction), transaction->target2initiator_buffer_size, SERIAL_USART_TIMEOUT)) {
            SDPRINTLN("rx payload fail");
            recover_after_error();
            return false;
        }
    }

    return true;
}
