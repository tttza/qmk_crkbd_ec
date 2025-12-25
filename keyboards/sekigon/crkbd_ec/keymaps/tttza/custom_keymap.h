typedef union {
    uint32_t raw;
    struct {
        bool jis    :1;
    };
} user_config_t;


void load_persistent(void);
void save_persistent(void);
void set_keyboard_lang_to_jis(bool);
