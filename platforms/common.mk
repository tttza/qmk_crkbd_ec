PLATFORM_COMMON_DIR = $(PLATFORM_PATH)/$(PLATFORM_KEY)

SRC +=	\
	$(PLATFORM_PATH)/suspend.c \
	$(PLATFORM_PATH)/synchronization_util.c \
	$(PLATFORM_PATH)/timer.c \
	$(PLATFORM_COMMON_DIR)/hardware_id.c \
	$(PLATFORM_COMMON_DIR)/platform.c \
	$(PLATFORM_COMMON_DIR)/suspend.c \
	$(PLATFORM_COMMON_DIR)/timer.c \
	$(PLATFORM_COMMON_DIR)/bootloaders/$(BOOTLOADER_TYPE).c

# Use platform-specific suspend/timer for Pico and drop the generic copies to avoid duplicate symbols
ifeq ($(PLATFORM_KEY),pico)
SRC := $(filter-out $(PLATFORM_PATH)/suspend.c $(PLATFORM_PATH)/timer.c,$(SRC))
endif

# Search Path
VPATH += $(PLATFORM_PATH)
VPATH += $(PLATFORM_PATH)/$(PLATFORM_KEY)
VPATH += $(PLATFORM_PATH)/$(PLATFORM_KEY)/$(DRIVER_DIR)
