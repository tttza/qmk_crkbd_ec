BOOTLOADER = rp2040

CUSTOM_MATRIX = lite
SRC += analog.c ec_switch_matrix.c matrix.c
CFLAGS += -DPLATFORM_PICO=1

# Force the legacy Pico SDK platform (matches working rp2040 branch).
PLATFORM_KEY = pico
MCU_FAMILY = PICO
MCU_SERIES = RP2040
MCU = cortex-m0plus
EEPROM_DRIVER = wear_leveling
WEAR_LEVELING_DRIVER = rp2040_flash

SPLIT_KEYBOARD = yes
RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = no   # Temporarily disable to avoid ChibiOS-only WS2812 driver on pico platform
# RGB_MATRIX_DRIVER = ws2812


VIA_ENABLE = yes
# Match the old rp2040 branch behavior but stick to supported boolean.
BOOTMAGIC_ENABLE = no       # Virtual DIP switch configuration
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

ifeq ($(HANDEDNESS),right)
	SRC += handedness_right.c
else
	SRC += handedness_left.c
endif

