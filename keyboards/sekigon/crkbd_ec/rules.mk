
CUSTOM_MATRIX = lite
SRC += analog.c ec_switch_matrix.c matrix.c
CFLAGS += -DPLATFORM_PICO=1

MCU_FAMILY = PICO
MCU_SERIES = RP2040
MCU = cortex-m0plus

SPLIT_KEYBOARD = yes
RGBLIGHT_ENABLE = no
RGB_MATRIX_ENABLE = yes
RGB_MATRIX_DRIVER = WS2812

# Build Options
#   change yes to no to disable
#
VIA_ENABLE = yes
BOOTMAGIC_ENABLE = lite     # Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes       # Mouse keys
EXTRAKEY_ENABLE = yes       # Audio control and System control
CONSOLE_ENABLE = yes        # Console for debug
COMMAND_ENABLE = no         # Commands for debug and configuration
# Do not enable SLEEP_LED_ENABLE. it uses the same timer as BACKLIGHT_ENABLE
SLEEP_LED_ENABLE = no       # Breathing sleep LED during USB suspend
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

