
TARGET := $(TARGET)_pico_micro
MCU_FAMILY := PICO
MCU_SERIES := RP2040
MCU := cortex-m0plus

OPT_DEFS += -DCONVERT_TO_PICO_MICRO
