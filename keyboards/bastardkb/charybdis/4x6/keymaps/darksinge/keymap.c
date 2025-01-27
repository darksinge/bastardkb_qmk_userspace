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
    LAYER_POINTER,
    LAYER_RAISE,
    LAYER_GAMING,
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
#define AMETHYST S(KC_LALT)
#define PT_Z LT(LAYER_POINTER, KC_Z)
#define PT_SLSH LT(LAYER_POINTER, KC_SLSH)
#define S_MS3 S(KC_BTN3)

#ifndef POINTING_DEVICE_ENABLE
#    define DRGSCRL KC_NO
#    define DPI_MOD KC_NO
#    define DPI_RMOD KC_NO
#    define S_D_MOD KC_NO
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

enum {
    TD_AMETHYST,
};

tap_dance_action_t tap_dance_actions[] = {
    [TD_AMETHYST] = ACTION_TAP_DANCE_DOUBLE(AMETHYST, C(AMETHYST)),
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  [LAYER_BASE] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       KC_ESC,    KC_1,    KC_2,    KC_3,    KC_4,    KC_5,       KC_6,    KC_7,   KC_8,   KC_9,   KC_0,    KC_MINS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_TAB,    KC_Q,    KC_W,    KC_E,    KC_R,    KC_T,       KC_Y,    KC_U,   KC_I,   KC_O,   KC_P,    KC_BSLS,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_MINS,   CTL_A,   ALT_S,   GUI_D,   KC_F,    KC_G,       KC_H,    KC_J,   KC_K,   KC_L,   CTL_SCLN, KC_QUOT,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       POINTER,   KC_Z,   KC_X,    KC_C,    KC_V,    KC_B,       KC_N,    KC_M,  KC_COMM, KC_DOT, KC_SLSH, TT(POINTER),
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_LSFT, LOWER,   KC_ENT,       KC_SPC,  KC_BSPC,
                                           KC_LCTL, KC_LGUI,      AMETHYST
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),

  [LAYER_LOWER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       TOHOME,  KC_F1,   KC_F2,    KC_F3,   KC_F4,   KC_F5,     KC_F6,   KC_F7,    KC_F8,    KC_F9,   KC_F10, KC_F11,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       RGB_MOD,  KC_NO,   KC_AT,   KC_LCBR, KC_RCBR, KC_ENT,    KC_UNDS,  KC_PLUS, KC_ASTR,  KC_EXLM, KC_RBRC, KC_F12,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       RGB_TOG,  KC_HASH, KC_DLR,  KC_LPRN, KC_RPRN, KC_TAB,    KC_MINS,  KC_EQL,   KC_GT,   KC_PIPE, KC_TILD, KC_SLSH,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       KC_DEL, KC_PERC, KC_CIRC, KC_LBRC, KC_RBRC, KC_GRAVE,        KC_AMPR, KC_LEFT, KC_DOWN, KC_UP, KC_RIGHT, GAMING,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                            C(KC_UP), XXXXXXX, KC_CAPS,           KC_SPC, KC_ESC,
                                           XXXXXXX, XXXXXXX,      KC_ENT
  //                            ╰───────────────────────────╯ ╰──────────────────╯
  ),



  [LAYER_POINTER] = LAYOUT(
  // ╭──────────────────────────────────────────────────────╮ ╭──────────────────────────────────────────────────────╮
       TOHOME,  XXXXXXX, XXXXXXX, XXXXXXX, EE_CLR, QK_BOOT,     QK_BOOT, EE_CLR, XXXXXXX, XXXXXXX, XXXXXXX,  XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, G(KC_Q), G(KC_W), XXXXXXX, XXXXXXX, XXXXXXX,    XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       SNP_TOG, G(KC_A), G(KC_S), G(KC_D), KC_LSFT, S_D_MOD,    DPI_MOD, SNP_TOG, XXXXXXX, XXXXXXX, XXXXXXX, XXXXXXX,
  // ├──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────┤
       XXXXXXX, G(KC_Z), G(KC_X), G(KC_C), G(KC_V), S_D_RMOD,   DPI_RMOD, DRG_TOG, KC_BTN5, KC_BTN4, KC_BTN1, TOHOME,
  // ╰──────────────────────────────────────────────────────┤ ├──────────────────────────────────────────────────────╯
                                  KC_BTN1, KC_BTN2, KC_BTN3,    KC_BTN5, KC_BTN4,
                                           DRGSCRL, S_MS3,      KC_BTN1
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
};
// clang-format on

// #ifdef RGBLIGHT_LAYERS
// const rgblight_segment_t PROGMEM base_layer[]    = RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_GREEN});
// const rgblight_segment_t PROGMEM lower_layer[]   = RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_BLUE});
// const rgblight_segment_t PROGMEM pointer_layer[] = RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_PURPLE});
// const rgblight_segment_t PROGMEM raise_layer[]   = RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_RED});
// const rgblight_segment_t PROGMEM gaming_layer[]  = RGBLIGHT_LAYER_SEGMENTS({0, 20, HSV_CYAN});

// const rgblight_segment_t* const PROGMEM my_rgb_layers[] = RGBLIGHT_LAYERS_LIST(base_layer, lower_layer, pointer_layer, raise_layer, gaming_layer);

// #endif // RGBLIGHT_LAYERS

// void keyboard_post_init_user(void) {
//     // keyboard_post_init_rgb();
// #ifdef RGBLIGHT_LAYERS
//     rgblight_layers = my_rgb_layers;
// #else
//     rgblight_sethsv_noeeprom(HSV_BLUE);
// #endif
//     /*debug_enable=true;*/
//     /*debug_matrix=true;*/
// }

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
#endif
