
COMMON_VPATH += $(DRIVER_PATH)/pico

PROTOCOL_DIR := $(TMK_PATH)/protocol
PICO_PROTOCOL_DIR := $(PROTOCOL_DIR)/pico

# The boot2 section is supplied either by the legacy C blob or the generated bs2_default.
ifeq ($(RP2040_BOOT2_USE_LEGACY),yes)
	# stage2_bootloaders.c provides .boot2 when legacy is enabled
else
	SRC += $(INTERMEDIATE_OUTPUT)/src/bs2_default_padded_checksummed.S
endif

LDSCRIPT := $(PICO_SDK_PATH)/src/rp2_common/pico_standard_link/memmap_default.ld

SRC += $(PICO_PROTOCOL_DIR)/protocol.c
SRC += $(PICO_PROTOCOL_DIR)/usb_descriptors.c
SRC += $(PICO_PROTOCOL_DIR)/qmk_main.c
SRC += $(PICO_PROTOCOL_DIR)/iusb.c
SRC += $(PICO_PROTOCOL_DIR)/pio_manager.c
SRC += $(PICO_PROTOCOL_DIR)/usb_util.c

CFLAGS += -DPROTOCOL_PICO
CFLAGS += -DGPIO_INPUT_PIN_DELAY=100
CFLAGS += -DDYNAMIC_KEYMAP_EEPROM_MAX_ADDR=8191

VPATH += $(PROTOCOL_DIR)
VPATH += $(PICO_PROTOCOL_DIR)
VPATH += $(PICO_PROTOCOL_DIR)/lufa_utils

ifeq ($(strip $(MIDI_ENABLE)), yes)
	include $(TMK_PATH)/protocol/midi.mk
endif

