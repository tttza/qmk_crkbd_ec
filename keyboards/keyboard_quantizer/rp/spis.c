/**
 * Copyright (c) 2020 Raspberry Pi (Trading) Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include "spis.h"
#include "debug.h"

#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/spi.h"
#include "hardware/dma.h"
#include "hardware/irq.h"

static uint8_t        txbuf[SPI_BUFFER_SIZE];
static uint8_t        rxbuf[SPI_BUFFER_SIZE];
static uint           dma_tx;
static uint           dma_rx;
static spis_handler_t s_spis_handler = NULL;

void spis_start(void) {
    txbuf[0] = 0xfe;
    dma_channel_set_read_addr(dma_tx, txbuf, false);
    dma_channel_set_write_addr(dma_rx, rxbuf, false);

    dma_start_channel_mask((1u << dma_tx) | (1u << dma_rx));
}

static void cs_handler(uint gpio, uint32_t event) {
    dma_channel_abort(dma_rx);
    dma_channel_abort(dma_tx);
    if (s_spis_handler != NULL) {
        s_spis_handler(
            rxbuf, (uint32_t)dma_hw->ch[dma_rx].write_addr - (uint32_t)rxbuf,
            txbuf);
    }
}

static inline void spis_interrupt_handler(void) {
    // Clear the interrupt request.
    dma_hw->ints0 = 1u << dma_rx;
    if (s_spis_handler != NULL) {
        s_spis_handler(
            rxbuf, (uint32_t)dma_hw->ch[dma_rx].write_addr - (uint32_t)rxbuf,
            txbuf);
    }
}

void spis_init(spis_handler_t spis_handler) {
    s_spis_handler = spis_handler;

    // Enable SPI at 11 MHz and connect to GPIOs
    spi_init(KQ_SPI, 11000 * 1000);
    spi_set_slave(KQ_SPI, true);
    spi_set_format(KQ_SPI, 8, SPI_CPOL_1, SPI_CPHA_1, SPI_MSB_FIRST);
    gpio_set_function(KQ_PIN_SCK, GPIO_FUNC_SPI);
    gpio_set_function(KQ_PIN_SPI_TX, GPIO_FUNC_SPI);
    gpio_set_function(KQ_PIN_SPI_RX, GPIO_FUNC_SPI);
    gpio_set_function(KQ_PIN_CS, GPIO_FUNC_SPI);
    // Make the SPI pins available to picotool
    bi_decl(bi_4pins_with_func(KQ_PIN_SCK, KQ_PIN_SPI_TX, KQ_PIN_SPI_RX,
                               KQ_PIN_CS, GPIO_FUNC_SPI));

    // Grab some unused dma channels
    dma_tx = dma_claim_unused_channel(true);
    dma_rx = dma_claim_unused_channel(true);

    // We set the outbound DMA to transfer from a memory buffer to the SPI
    // transmit FIFO paced by the SPI TX FIFO DREQ The default is for the read
    // address to increment every element (in this case 1 byte - DMA_SIZE_8) and
    // for the write address to remain unchanged.

    printf("Configure TX DMA\n");
    dma_channel_config c = dma_channel_get_default_config(dma_tx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(
        &c, spi_get_index(KQ_SPI) ? DREQ_SPI1_TX : DREQ_SPI0_TX);
    channel_config_set_read_increment(&c, true);
    channel_config_set_write_increment(&c, false);
    dma_channel_configure(dma_tx, &c,
                          &spi_get_hw(KQ_SPI)->dr,  // write address
                          txbuf,                    // read address
                          SPI_BUFFER_SIZE,  // element count (each element is of
                                            // size transfer_data_size)
                          false);           // don't start yet

    memset(txbuf, 0xff, sizeof(txbuf));

    printf("Configure RX DMA\n");

    // We set the inbound DMA to transfer from the SPI receive FIFO to a memory
    // buffer paced by the SPI RX FIFO DREQ We coinfigure the read address to
    // remain unchanged for each element, but the write address to increment (so
    // data is written throughout the buffer)
    c = dma_channel_get_default_config(dma_rx);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_8);
    channel_config_set_dreq(
        &c, spi_get_index(KQ_SPI) ? DREQ_SPI1_RX : DREQ_SPI0_RX);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    dma_channel_configure(dma_rx, &c,
                          rxbuf,                    // write address
                          &spi_get_hw(KQ_SPI)->dr,  // read address
                          SPI_BUFFER_SIZE,  // element count (each element is of
                                            // size transfer_data_size)
                          false);           // don't start yet

    gpio_set_irq_enabled_with_callback(KQ_PIN_CS, GPIO_IRQ_EDGE_RISE, true,
                                       cs_handler);
}
