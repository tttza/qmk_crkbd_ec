# MCU name
MCU_FAMILY = PICO
MCU_SERIES = RP2040
MCU = cortex-m0plus

VIA_ENABLE = yes

# Build Options
#   comment out to disable the options.
#
BOOTMAGIC_ENABLE = yes	# Virtual DIP switch configuration
MOUSEKEY_ENABLE = yes	  # Mouse keys
EXTRAKEY_ENABLE = yes	  # Audio control and System control
CONSOLE_ENABLE = no	    # Console for debug
COMMAND_ENABLE = no     # Commands for debug and configuration
SLEEP_LED_ENABLE = no   # Breathing sleep LED during USB suspend
NKRO_ENABLE = no		    # USB Nkey Rollover - if this doesn't work, see here: https://github.com/tmk/tmk_keyboard/wiki/FAQ#nkro-doesnt-work
BACKLIGHT_ENABLE = no  # Enable keyboard backlight functionality
AUDIO_ENABLE = no
RGBLIGHT_ENABLE = no

LAYOUTS = ortho_3x10

# Disable unsupported hardware
RGBLIGHT_SUPPORTED = no
AUDIO_SUPPORTED = no
