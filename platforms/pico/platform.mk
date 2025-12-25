# Legacy Pico SDK platform glue
PLATFORM_SUPPORTED_BOOTLOADERS := rp2040

# Ensure headers under tmk_core/common/pico are visible
EXTRAINCDIRS += $(TMK_PATH)/common/pico
EXTRAINCDIRS += $(DRIVER_PATH)/pico

# Reuse RP2040 wear-leveling driver provided under chibios
EXTRAINCDIRS += $(PLATFORM_PATH)/chibios/drivers
COMMON_VPATH += $(PLATFORM_PATH)/chibios/drivers/wear_leveling

# Reuse the legacy Pico toolchain and sources
include $(TMK_PATH)/pico.mk

# Prefer Pico SDK printf over the tiny printf fallback to avoid duplicate symbols
SRC := $(filter-out lib/printf/src/printf/printf.c printf.c,$(SRC))

# Ensure the correct bootloader source is included (common.mk may add an empty path before BOOTLOADER_TYPE is set)
SRC := $(filter-out $(PLATFORM_COMMON_DIR)/bootloaders/.c,$(SRC))
SRC += $(PLATFORM_COMMON_DIR)/bootloaders/$(BOOTLOADER_TYPE).c