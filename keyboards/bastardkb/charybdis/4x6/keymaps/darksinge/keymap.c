/**
 * Copyright 2021 Charly Delay <charly@codesink.dev> (@0xcharly)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
#    include "timer.h"
#endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

enum charybdis_keymap_layers {
    LAYER_BASE = 0,
    LAYER_LOWER,
    LAYER_DAVINCI_RESOLVE,
    LAYER_POINTER,
    LAYER_RAISE,
    LAYER_GAMING,
    LAYER_DANGER,
};

/** \brief Automatically enable sniping-mode on the pointer layer. */
#define CHARYBDIS_AUTO_SNIPING_ON_LAYER LAYER_POINTER

#ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
static uint16_t auto_pointer_layer_timer = 0;

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS 1000
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS

#    ifndef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#        define CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD 8
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD
#endif     // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#define TOHOME TO(LAYER_BASE)
#define LOWER MO(LAYER_LOWER)
#define RAISE MO(LAYER_RAISE)
#define POINTER MO(LAYER_POINTER)
#define GAMING TO(LAYER_GAMING)
#define TODV TO(LAYER_DAVINCI_RESOLVE)
#define TODANGER MO(LAYER_DANGER)
#define AMETHYST S(KC_LALT)
#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)
#define S_MS3 S(KC_BTN3)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define DPI_RMOD KC_NO
#    define S_D_MOD KC_NO
#    define S_D_RMOD KC_NO
#    define SNIPING KC_NO
#    define DRG_TOG KC_NO
#    define SNP_TOG KC_NO
#endif // !POINTING_DEVICE_ENABLE

// Left-hand home row mods
#define CTL_A LCTL_T(KC_A)
#define ALT_S LALT_T(KC_S)
#define GUI_D LGUI_T(KC_D)

#define ALT_Z LALT_T(KC_Z)

// Right-hand home row mods
#define GUI_K RGUI_T(KC_K)
#define ALT_L LALT_T(KC_L)
#define CTL_SCLN RCTL_T(KC_SCLN)

enum tap_dance_actions {
    TD_AMYST,
};

enum custom_keycodes {
    VI_SLCT_BLK = SAFE_RANGE,
};

typedef struct {
    uint16_t tap;
    uint16_t hold;
    uint16_t held;
} tap_dance_tap_hold_t;

// Forward declarations of tap dance functions
void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data);
void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data);

#define ACTION_TAP_DANCE_TAP_HOLD(tap, hold)                                        \
    {                                                                               \
        .fn        = {NULL, tap_dance_tap_hold_finished, tap_dance_tap_hold_reset}, \
        .user_data = (void *)&((tap_dance_tap_hold_t){tap, hold, 0}),               \
    }

tap_dance_action_t tap_dance_actions[] = {
    [TD_AMYST] = ACTION_TAP_DANCE_TAP_HOLD(KC_ESC, AMETHYST),
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    uint8_t             current_layer;
    tap_dance_action_t *action;

    switch (keycode) {
        case TD(TD_AMYST):
            action = &tap_dance_actions[QK_TAP_DANCE_GET_INDEX(keycode)];
            if (!record->event.pressed && action->state.count && !action->state.finished) {
                tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)action->user_data;
                tap_code16(tap_hold->tap);
            }
            break;
        case VI_SLCT_BLK:
            if (record->event.pressed) {
                SEND_STRING("V$%");
            }
            break;
#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
        case KC_BTN3:
            current_layer = get_highest_layer(layer_state);
            if (get_mods() == MOD_BIT(KC_LSFT)) {
                if (record->event.pressed) {
                    charybdis_set_pointer_sniping_enabled(false);
                } else {
                    charybdis_set_pointer_sniping_enabled(layer_state_cmp(current_layer, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
                }
            }
            break;
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE
    }
    return true;
};

void tap_dance_tap_hold_finished(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (state->pressed) { // If key is being held
        if (state->count == 1
#ifndef PERMISSIVE_HOLD
            && !state->interrupted
#endif
        ) {
            add_mods(MOD_BIT(KC_LSFT) | MOD_BIT(KC_LALT));
            tap_hold->held = tap_hold->hold;
        } else {
            register_code16(tap_hold->tap); // Register tap action (KC_ESC)
            tap_hold->held = tap_hold->tap;
        }
    }
}

void tap_dance_tap_hold_reset(tap_dance_state_t *state, void *user_data) {
    tap_dance_tap_hold_t *tap_hold = (tap_dance_tap_hold_t *)user_data;

    if (tap_hold->held) {
        del_mods(MOD_BIT(KC_LSFT) | MOD_BIT(KC_LALT));
        // unregister_code16(tap_hold->held); // Unregister the last registered key
        tap_hold->held = 0;
    }
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,       KC_6,    KC_7,   KC_8,   KC_9,   KC_0,    KC_PLUS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,   KC_I,   KC_O,   KC_P,    KC_BSLS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MINS,   CTL_A,   ALT_S,   GUI_D,   KC_F,    KC_G,       KC_H,    KC_J,   GUI_K,   ALT_L,   CTL_SCLN, KC_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       POINTER,   KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH, TT(POINTER),
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_LSFT, LOWER,   KC_ENT,       KC_SPC,  KC_BSPC,
                                           KC_LGUI, KC_LCTL,      AMETHYST
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_LOWER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       C(KC_UP),  KC_F1,   KC_F2,    KC_F3,   KC_F4,   KC_F5,     KC_F6,   KC_F7,    KC_F8,    KC_F9,   KC_F10, KC_F11,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       RGB_MOD, S(KC_ENT), KC_AT, KC_LCBR, KC_RCBR, VI_SLCT_BLK, S(A(KC_MINS)),  KC_PLUS, KC_ASTR,  KC_EXLM, KC_RBRC, KC_F12,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       RGB_TOG,  KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN, KC_TAB,      KC_MINS,  KC_EQL,   KC_GT,   KC_PIPE, KC_TILD, KC_SLSH,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_DEL, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_GRAVE,      KC_AMPR, KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, GAMING,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                 C(KC_UP), _______, KC_CAPS,      KC_SPC, XXXXXXX,
                                           XXXXXXX, TODANGER,    S(KC_ENT)
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_DAVINCI_RESOLVE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       KC_ESC,  KC_F1,  KC_F2,  KC_F3,  KC_F4,  KC_T,            KC_F6,    KC_F7,   KC_F8,   KC_F9,   KC_F10,    TOHOME,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_D, KC_N, C(G(KC_L)), G(A(KC_L)), A(KC_Y), A(KC_X),         KC_Y,    KC_U,   KC_I,   KC_O,   KC_P,    KC_BSLS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       G(KC_R),   KC_A,   C(G(KC_LBRC)),   G(KC_B),   C(G(KC_RBRC)),    S(KC_BSPC),          KC_H,    KC_J,   KC_K,   KC_L,   KC_SCLN, KC_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       POINTER,   KC_Z,   KC_J,    KC_SPC,    KC_L,    KC_BSPC,       G(KC_C),    A(KC_V),  KC_COMM, KC_DOT, KC_SLSH, KC_LGUI,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_LSFT, A(KC_V),   KC_ENT,      KC_SPC,  KC_BSPC,
                                           KC_LGUI, KC_LCTL,       KC_LALT
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_POINTER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       TOHOME,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, QK_BOOT,     QK_BOOT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,  TODV,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, G(KC_Q), G(KC_W), XXXXXXX, DPI_MOD, DPI_RMOD,    KC_PLUS, KC_7, KC_8, KC_9, KC_ASTR, KC_SLSH,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, G(KC_A), G(KC_S), XXXXXXX, G(KC_F), S_D_MOD,     KC_MINS, KC_4, KC_5, KC_6, KC_ENT, KC_BTN1,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       _______, DRGSCRL, G(KC_X), G(KC_C), G(KC_V), S_D_RMOD,     KC_0,   KC_1, KC_2, KC_3, KC_DOT, SNP_TOG,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_BTN1, KC_BTN2, KC_BTN3,    KC_BTN5, KC_BTN4,
                                           S_MS3, KC_LOPT,      KC_LSFT
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  // NOTE: Not using this layer, but might keep it for future use.
  [LAYER_RAISE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
        KC_F12,   KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,      KC_F6,   KC_F7,   KC_F8,   KC_F9,  KC_F10,  KC_F11,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MNXT, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_VOLU,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MPLY, KC_LEFT,   KC_UP, KC_DOWN, KC_RGHT, XXXXXXX,    XXXXXXX, KC_RSFT, KC_RCTL, KC_RALT, KC_RGUI, KC_MUTE,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MPRV, KC_HOME, KC_PGUP, KC_PGDN,  KC_END, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, KC_VOLD,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  _______, _______, XXXXXXX,    _______, XXXXXXX,
                                           _______, _______,    XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),


  [LAYER_GAMING] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,       KC_6,    KC_7,   KC_8,   KC_9,   KC_0,    KC_MINS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,   KC_I,   KC_O,   KC_P,    KC_BSLS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MINS,   KC_A,   KC_S,   KC_D,   KC_F,    KC_G,          KC_H,    KC_J,   KC_K,   KC_L,   KC_SCLN, KC_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       POINTER,   KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH, KC_LGUI,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_LSFT, KC_SPC,   KC_ENT,      KC_SPC,  KC_BSPC,
                                           KC_LCTL, KC_LALT,       TOHOME
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_DANGER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, EE_CLR,  QK_BOOT,    QK_BOOT,  EE_CLR, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX,  XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX,
  //                                                👇🏻from TODANGER key on LAYER_LOWER
                                           XXXXXXX, _______,    XXXXXXX
  //                            ╰───────────────────────────╯ ╰──────────────────╯
    )
};
// clang-format on

#ifdef POINTING_DEVICE_ENABLE
#    ifdef CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE
report_mouse_t pointing_device_task_user(report_mouse_t mouse_report) {
    if (abs(mouse_report.x) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD || abs(mouse_report.y) > CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_THRESHOLD) {
        if (auto_pointer_layer_timer == 0) {
            layer_on(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
            rgb_matrix_mode_noeeprom(RGB_MATRIX_NONE);
            rgb_matrix_sethsv_noeeprom(HSV_GREEN);
#        endif // RGB_MATRIX_ENABLE
        }
        auto_pointer_layer_timer = timer_read();
    }
    return mouse_report;
}

void matrix_scan_user(void) {
    if (auto_pointer_layer_timer != 0 && TIMER_DIFF_16(timer_read(), auto_pointer_layer_timer) >= CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_TIMEOUT_MS) {
        auto_pointer_layer_timer = 0;
        layer_off(LAYER_POINTER);
#        ifdef RGB_MATRIX_ENABLE
        rgb_matrix_mode_noeeprom(RGB_MATRIX_DEFAULT_MODE);
#        endif // RGB_MATRIX_ENABLE
    }
}
#    endif // CHARYBDIS_AUTO_POINTER_LAYER_TRIGGER_ENABLE

#    ifdef CHARYBDIS_AUTO_SNIPING_ON_LAYER
layer_state_t layer_state_set_user(layer_state_t state) {
    charybdis_set_pointer_sniping_enabled(layer_state_cmp(state, CHARYBDIS_AUTO_SNIPING_ON_LAYER));
    return state;
}
#    endif // CHARYBDIS_AUTO_SNIPING_ON_LAYER
#endif     // POINTING_DEVICE_ENABLE

#ifdef RGB_MATRIX_ENABLE
// Forward-declare this helper function since it is defined in rgb_matrix.c.
void rgb_matrix_update_pwm_buffers(void);
#endif // RGB_MATRIX_ENABLE

bool rgb_matrix_indicators_user(void) {
    uint8_t layer = get_highest_layer(layer_state); // Retrieve the current layer

    hsv_t red    = {0, 255, 255};
    hsv_t green  = {85, 255, 255};
    hsv_t blue   = {170, 255, 255};
    hsv_t cyan   = {128, 255, 255};
    hsv_t orange = {15, 255, 255};
    // hsv_t yellow = {43, 255, 255};
    // hsv_t pink   = {234, 255, 255};
    // hsv_t teal   = {150, 255, 255};
    // hsv_t amber  = {36, 255, 255};
    // hsv_t indigo = {190, 255, 255};
    // hsv_t lime   = {64, 255, 255};

    hsv_t hsv_white = {0, 0, 255};
    /* hsv_white.v     = RGB_MATRIX_MAXIMUM_BRIGHTNESS; */
    hsv_white.v     = RGB_MATRIX_MAXIMUM_BRIGHTNESS / 2;
    rgb_t rgb_white = hsv_to_rgb(hsv_white);

    hsv_t hsv;
    switch (layer) {
        case LAYER_BASE:
            hsv = red;
            break;
        case LAYER_LOWER:
            hsv = blue;
            break;
        case LAYER_POINTER:
            hsv = green;
            break;
        case LAYER_RAISE:
        case LAYER_GAMING:
            hsv = red;
            break;
        case LAYER_DAVINCI_RESOLVE:
            hsv = cyan;
            break;
        case LAYER_DANGER:
            hsv = red;
            break;
        default:
            hsv = blue;
            break;
    }

    hsv.v = RGB_MATRIX_MAXIMUM_BRIGHTNESS / 3;

    uint8_t arrow_key_indexes[] = {48, 41, 40, 33};
    uint8_t todanger_key_index = 24; // TODANGER key position
    /* uint8_t davinci_key_indexes[] = {52, 25}; // Special keys for DAVINCI_RESOLVE layer */

    rgb_t rgb = hsv_to_rgb(hsv);
    rgb_t rgb_orange = hsv_to_rgb(orange);

    for (int i = 0; i < 56; i++) {
        if (layer == LAYER_LOWER) {
            bool is_arrow_key = false;
            for (int j = 0; j < sizeof(arrow_key_indexes) / sizeof(arrow_key_indexes[0]); j++) {
                if (i == arrow_key_indexes[j]) {
                    rgb_matrix_set_color(i, rgb_white.r, rgb_white.g, rgb_white.b);
                    is_arrow_key = true;
                    break;
                }
            }
            if (!is_arrow_key) {
                if (i == todanger_key_index) {
                    rgb_matrix_set_color(i, rgb_orange.r, rgb_orange.g, rgb_orange.b);
                } else {
                    rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
                }
            }
        } else if (layer == LAYER_DAVINCI_RESOLVE) {
            bool is_davinci_key = false;
            /* for (int j = 0; j < sizeof(davinci_key_indexes) / sizeof(davinci_key_indexes[0]); j++) { */
            /*     if (i == davinci_key_indexes[j]) { */
            /*         rgb_matrix_set_color(i, rgb_blue.r, rgb_blue.g, rgb_blue.b); */
            /*         is_davinci_key = true; */
            /*         break; */
            /*     } */
            /* } */
            if (!is_davinci_key) {
                rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
            }
        } else {
            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }


    return true;
}
