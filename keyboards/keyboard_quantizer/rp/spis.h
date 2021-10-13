
#pragma once

#include <stdint.h>

#define KQ_SPI spi1
#define KQ_PIN_SCK 26
#define KQ_PIN_SPI_TX 27
#define KQ_PIN_SPI_RX 28
#define KQ_PIN_CS 29
#define SPI_BUFFER_SIZE 1024

typedef void(*spis_handler_t)(uint8_t const * received, uint32_t receive_len, uint8_t * next_send);

void spis_init(spis_handler_t spis_handler);
void spis_start(void);
