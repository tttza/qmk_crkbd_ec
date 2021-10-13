
#pragma once

#define DYNAMIC_KEYMAP_LAYER_COUNT 10

#undef PRODUCT_ID
#define PRODUCT_ID 0x999b

/* key matrix size */
#define MATRIX_ROWS 24
#define MATRIX_COLS 8
#define MATRIX_ROWS_DEFAULT MATRIX_ROWS
#define MATRIX_COLS_DEFAULT MATRIX_COLS
#define MATRIX_MODIFIER_ROW 21
#define MATRIX_MSBTN_ROW 22
#define MATRIX_MSGES_ROW 23
#define MATRIX_MSGES_ROW 23
#define MATRIX_MSWHEEL_ROW 23
#define MATRIX_MSWHEEL_COL 4
#define IS_LEFT_HAND true

#define OVERRIDE_KEYMAP_KEY_TO_KEYCODE
#define QUANTIZER_REPORT_PARSER REPORT_PARSER_DEFAULT

#define RGBLIGHT_SPLIT
#define G00 255 // Dummy for bin/qmk config generation
#define RGB_DI_PIN G00
#ifdef RGB_DI_PIN
#    define RGBLED_NUM_DEFAULT 128
#    define RGBLIGHT_HUE_STEP 8
#    define RGBLIGHT_SAT_STEP 8
#    define RGBLIGHT_VAL_STEP 8
#    define RGBLIGHT_LIMIT_VAL 255 /* The maximum brightness level */
#    define RGBLIGHT_SLEEP /* If defined, the RGB lighting will be switched \
                              off when the host goes to sleep */
                           /*== all animations enable ==*/
#    define RGBLIGHT_ANIMATIONS
/*== or choose animations ==*/
#    define RGBLIGHT_EFFECT_BREATHING
#    define RGBLIGHT_EFFECT_RAINBOW_MOOD
#    define RGBLIGHT_EFFECT_RAINBOW_SWIRL
#    define RGBLIGHT_EFFECT_SNAKE
#    define RGBLIGHT_EFFECT_KNIGHT
#    define RGBLIGHT_EFFECT_CHRISTMAS
#    define RGBLIGHT_EFFECT_STATIC_GRADIENT
#    define RGBLIGHT_EFFECT_RGB_TEST
#    define RGBLIGHT_EFFECT_ALTERNATING
/*== customize breathing effect ==*/
/*==== (DEFAULT) use fixed table instead of exp() and sin() ====*/
#    define RGBLIGHT_BREATHE_TABLE_SIZE 256  // 256(default) or 128 or 64
/*==== use exp() and sin() ====*/
#    define RGBLIGHT_EFFECT_BREATHE_CENTER 1.85  // 1 to 2.7
#    define RGBLIGHT_EFFECT_BREATHE_MAX 255      // 0 to 255
#endif
