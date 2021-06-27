
COMMON_VPATH += $(DRIVER_PATH)/pico

PROTOCOL_DIR := $(TMK_PATH)/protocol/pico

## TODO Generate by makefile
SRC += $(PROTOCOL_DIR)/bs2_default_padded_checksummed.S
LDSCRIPT := $(PICO_SDK_PATH)/src/rp2_common/pico_standard_link/memmap_default.ld

SRC += $(PROTOCOL_DIR)/main.c
SRC += $(PROTOCOL_DIR)/usb_descriptors.c
SRC += $(PROTOCOL_DIR)/msc_disk.c

CFLAGS += -DPROTOCOL_PICO

VPATH += $(TMK_PATH)/protocol
VPATH += $(PROTOCOL_DIR)
