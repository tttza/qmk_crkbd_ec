# MCU name
MCU_FAMILY = PICO
MCU_SERIES = RP2040
MCU = cortex-m0plus
SRC += pico_pio_usb/pio_usb.c pico_pio_usb/usb_crc.c
CFLAGS += -Ikeyboards/pico_pico_usb/pico_pio_usb

DEFAULT_FOLDER = pico_pico_usb/simple_split
