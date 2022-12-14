
COMMON_VPATH += $(DRIVER_PATH)/pico

PROTOCOL_DIR := $(TMK_PATH)/protocol/pico

SRC += $(KEYBOARD_OUTPUT)/src/bs2_default_padded_checksummed.S
LDSCRIPT := $(PICO_SDK_PATH)/src/rp2_common/pico_standard_link/memmap_default.ld

SRC += $(PROTOCOL_DIR)/protocol.c
SRC += $(PROTOCOL_DIR)/usb_descriptors.c
SRC += $(PROTOCOL_DIR)/qmk_main.c
SRC += $(PROTOCOL_DIR)/iusb.c
SRC += $(PROTOCOL_DIR)/pio_manager.c
SRC += $(PROTOCOL_DIR)/usb_util.c
SRC += $(PROTOCOL_DIR)/usbd.c

CFLAGS += -DPROTOCOL_PICO
CFLAGS += -DGPIO_INPUT_PIN_DELAY=100
CFLAGS += -DDYNAMIC_KEYMAP_EEPROM_MAX_ADDR=8191

VPATH += $(TMK_PATH)/protocol
VPATH += $(PROTOCOL_DIR)
VPATH += $(PROTOCOL_DIR)/lufa_utils

ifeq ($(strip $(MIDI_ENABLE)), yes)
	include $(TMK_PATH)/protocol/midi.mk
endif

