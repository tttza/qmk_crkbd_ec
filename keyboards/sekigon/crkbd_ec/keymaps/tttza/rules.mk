
CAPS_WORD_ENABLE = yes
AUTO_SHIFT_ENABLE = yes
LEADER_ENABLE = yes

SRC += custom_keymap.c
SRC += capsword_status.c
SRC += pico_debug.c
SRC += twpair_on_jis.c
SRC += select_word.c
SRC += leader_vim.c
SRC += xiao_status_service.c

# EXTRAFLAGS += -flto
