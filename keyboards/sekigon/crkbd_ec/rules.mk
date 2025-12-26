BOOTLOADER = rp2040

CUSTOM_MATRIX = lite
SRC += analog.c ec_switch_matrix.c matrix.c
# Ensure HID console print implementation is linked on RP2040.
SRC += tmk_core/common/pico/print.c
CFLAGS += -DPLATFORM_PICO=1
CFLAGS += -DPICO_BOOTSEL_VIA_DOUBLE_RESET=0
CFLAGS += -DPICO_WATCHDOG_TIMEOUT_MS=0
# Match the rp2040 branch default (preset 1: ~230400 baud) that previously worked.
OPT_DEFS += -DSELECT_SOFT_SERIAL_SPEED=1

# Use the SDK-generated bs2_default boot2 blob (legacy blob caused BOOTSEL).
RP2040_BOOT2_USE_LEGACY = no
OPT_DEFS += -DRP2040_FLASH_OLD_DEFAULT

# Force the legacy Pico SDK platform (matches working rp2040 branch).
PLATFORM_KEY = pico
MCU_FAMILY = PICO
MCU_SERIES = RP2040
MCU = cortex-m0plus
# Keep the wear-leveling layer but force the rp2040 flash backend to match the old branch.
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = rp2040_flash

SPLIT_KEYBOARD = yes
RGBLIGHT_ENABLE = no
# Re-enable RGB matrix (WS2812) now that split serial runs on PIO1.
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = ws2812
WS2812_DRIVER = custom

# Stick to the rp2040-branch PIO-backed soft serial driver.
SERIAL_DRIVER = bitbang


VIA_ENABLE = yes
# Match the old rp2040 branch behavior but stick to supported boolean.
BOOTMAGIC_ENABLE = yes      # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no # Breathing sleep LED during USB suspend
# if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
NKRO_ENABLE = no            # USB Nkey Rollover
BACKLIGHT_ENABLE = no       # Enable keyboard backlight functionality
BLUETOOTH_ENABLE = no       # Enable Bluetooth
AUDIO_ENABLE = no           # Audio output

# Keep compile-time handedness and seed EE_HANDS EEPROM early so split_pre_init
# sees the correct side even if EEPROM was stale.
ifeq ($(HANDEDNESS),right)
	SRC += handedness_right.c
	OPT_DEFS += -DINIT_EE_HANDS_RIGHT
	OPT_DEFS += -DMASTER_RIGHT
else
	SRC += handedness_left.c
	OPT_DEFS += -DINIT_EE_HANDS_LEFT
	OPT_DEFS += -DMASTER_LEFT
endif

