/* Copyright 2019 sekigon-gonnoc
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
#include "bmp.h"
#include "bmp_custom_keycode.h"
#include "keycode_str_converter.h"
#include "pointing_device.h"
#include "paw3204.h"
report_mouse_t mouse_rep;

enum click_state {
    NONE = 0,
    WAIT_CLICK,
    CLICKABLE,
    CLICKING,
    SCROLLING
};

enum custom_keycodes {
    KC_MY_BTN1 = BMP_SAFE_RANGE,
    KC_MY_BTN2,
    KC_MY_BTN3,
    KC_MY_SCR,
};

const key_string_map_t custom_keys_user = {.start_kc = KC_MY_BTN1, .end_kc = KC_MY_SCR, .key_strings = "MY_BTN1\0MY_BTN2\0MY_BTN3\0MY_SCR\0"};

enum click_state state;
uint16_t click_timer;

int16_t scroll_v_counter;
int16_t scroll_h_counter;

int16_t scroll_v_threshold = 30;
int16_t scroll_h_threshold = 30;

int16_t after_click_lock_movement = 0;

uint16_t click_layer = 9;

uint32_t keymaps_len() { return 38; }

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    LAYOUT(
        LT(6, KC_Q), KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, 
        KC_A, KC_S, LT(5, KC_D), KC_F, KC_G, KC_H, KC_J, LT(5, KC_K), KC_L, KC_ENT, 
        LSFT_T(KC_Z), LGUI_T(KC_X), KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, LCTL_T(KC_DOT), KC_BSPC, 
        KC_LCTL, KC_LGUI, LALT_T(KC_LANG2), LSFT_T(KC_TAB), KC_SPC, LT(1, KC_LANG1), KC_PGUP, KC_NO
    ),

    LAYOUT(
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, 
        LCTL_T(KC_EQL), KC_LBRC, KC_SLSH, KC_MINS, KC_RO, KC_SCLN, KC_QUOT, KC_RBRC, KC_NUHS, KC_JYEN, 
        KC_LSFT, KC_LGUI, KC_LALT, KC_LANG2, KC_LSFT, KC_SPC, KC_LANG1, KC_TRNS, KC_TRNS, KC_DEL, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    LAYOUT(LT(6, KC_Q), KC_W, KC_E, KC_R, KC_T, KC_Y, KC_U, KC_I, KC_O, KC_P, 
        KC_A, KC_S, LT(5, KC_D), KC_F, KC_G, KC_H, KC_J, LT(5, KC_K), KC_L, KC_ENT, 
        LSFT_T(KC_Z), LGUI_T(KC_X), KC_C, KC_V, KC_B, KC_N, KC_M, KC_COMM, LCTL_T(KC_DOT), KC_BSPC, 
        KC_TRNS, KC_TRNS, LALT_T(KC_LANG2), LSFT_T(KC_TAB), KC_SPC, LT(3, KC_LANG1), KC_TRNS, KC_TRNS
    ),

    LAYOUT(
        KC_1, KC_2, KC_3, KC_4, KC_5, KC_6, KC_7, KC_8, KC_9, KC_0, 
        KC_CIRC, KC_AT, KC_SLSH, KC_MINS, KC_UNDS, KC_SCLN, KC_COLN, KC_LBRC, KC_RBRC, KC_JYEN, 
        MO(4), KC_LGUI, KC_LALT, KC_LANG2, KC_LSFT, KC_SPC, KC_LANG1, KC_TRNS, KC_TRNS, KC_DEL, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    LAYOUT(
        KC_EXLM, KC_DQUO, KC_HASH, KC_DLR, KC_PERC, KC_AMPR, KC_QUOT, KC_LPRN, KC_RPRN, KC_0, 
        KC_TILD, KC_GRV, KC_QUES, KC_EQL, KC_UNDS, KC_PLUS, KC_ASTR, KC_LCBR, KC_RCBR, KC_PIPE, 
        KC_LSFT, KC_LGUI, KC_LALT, KC_LANG2, KC_LSFT, KC_SPC, KC_LANG1, KC_TRNS, KC_TRNS, KC_DEL, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    LAYOUT(
        KC_ESC, KC_TAB, KC_NO, KC_NO, KC_NO, KC_MS_BTN1, KC_MS_BTN2, KC_UP, KC_NO, KC_NO, 
        KC_LCTL, KC_TRNS, KC_QUES, KC_EXLM, KC_NO, KC_WH_U, KC_LEFT, KC_DOWN, KC_RGHT, KC_NO, 
        KC_LSFT, KC_LGUI, KC_LALT, KC_LANG2, KC_TRNS, KC_WH_D, KC_LANG1, KC_NO, KC_NO, KC_DEL, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    LAYOUT(
        KC_NO, KC_TAB, KC_NO, KC_NO, KC_F1, KC_F2, KC_F3, KC_F4, KC_F5, KC_F6, 
        KC_NO, KC_NO, KC_NO, KC_NO, KC_F7, KC_F8, KC_F9, KC_F10, KC_F11, KC_F12, 
        KC_LSFT, KC_NO, KC_NO, KC_NO, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO, MO(7), MO(8), 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    LAYOUT(
        RGB_TOG, RGB_MOD, RGB_HUI, RGB_SAI, RGB_VAI, KC_NO, KC_NO, KC_NO, DF(0), DF(2), 
        RGB_M_P, RGB_M_B, RGB_M_R, RGB_M_SW, RGB_M_SN, KC_NO, KC_NO, KC_NO, KC_NO, KC_NO, 
        RGB_M_K, RGB_M_X, RGB_M_G, KC_NO, KC_NO, RESET, KC_NO, KC_NO, KC_NO, KC_NO, 
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

     LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    ),

    LAYOUT(
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_MY_BTN1, KC_MY_SCR, KC_MY_BTN2, KC_MY_BTN3, KC_MS_R,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_WH_D, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,
        KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
    )
};

void on_mouse(void) {
    layer_on(click_layer);
    click_timer = timer_read();
    state = CLICKABLE;

    //dprintf("mouse on\n");
}

void off_mouse(void) {
    state = NONE;
    layer_off(click_layer);
    scroll_v_counter = 0;
    scroll_h_counter = 0;

    //dprintf("mouse off\n");
}

// #include <stdlib.h>しないために自前で絶対値を出す
// int16_t abs(int16_t num) {
//     if (num < 0) {
//         num = -num;
//     }

//     return num;
// }

bool is_mouse_mode(void) {
    return state == CLICKABLE || state == CLICKING || state == SCROLLING;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool continue_process = process_record_user_bmp(keycode, record);
    if (continue_process == false)
    {
        return false;
    }
    //dprintf("col:%4d  row:%4d keycode: %d \n", record->event.key.col, record->event.key.row, keycode);
    //dprintf("keycode: %d", keycode);
    switch (keycode) {
        case KC_MY_BTN1:
        case KC_MY_BTN2:
        case KC_MY_BTN3:
        {
            report_mouse_t currentReport = pointing_device_get_report();

            // どこのビットを対象にするか
            uint8_t btn = 1 << (keycode - KC_MY_BTN1);
            
            if (record->event.pressed) {
                // ビットORは演算子の左辺と右辺の同じ位置にあるビットを比較して、両方のビットのどちらかが「1」の場合に「1」にします。
                currentReport.buttons |= btn;
                mouse_rep.buttons |= btn;
                state = CLICKING;
                after_click_lock_movement = 30;
            } else {
                // ビットANDは演算子の左辺と右辺の同じ位置にあるビットを比較して、両方のビットが共に「1」の場合だけ「1」にします。
                currentReport.buttons &= ~btn;
                mouse_rep.buttons &= ~btn;
                on_mouse();
            }

            pointing_device_set_report(currentReport);
            return false;
        }

        case KC_MY_SCR:
            if (record->event.pressed) {
                state = SCROLLING;
            } else {
                off_mouse();
            }
         return false;

         default:
            if  (record->event.pressed) {
                state = NONE;
                layer_off(click_layer);
            }
        
    }
   
    return true;
}

    // void keyboard_post_init_user(void) {
    // // Customise these values to desired behaviour
    // debug_enable=true;
    // debug_matrix=true;
    // //debug_keyboard=true;
    // //debug_mouse=true;
    // }


void matrix_init_user() { init_paw3204(); }

void matrix_scan_user() {
    static int  cnt;
    static bool paw_ready;
    if (cnt++ % 50 == 0) {
        
        uint8_t pid = read_pid_paw3204();
        if (pid == 0x30) {
            //dprint("paw3204 OK\n");
            paw_ready = true;
        } else {
            //dprintf("paw3204 NG:%d\n", pid);
            paw_ready = false;
        }
        
    }

    if (paw_ready) {
        uint8_t stat;
        int8_t x, y;

        read_paw3204(&stat, &x, &y);
        //mouse_rep.buttons = 0;    // ボタンの状態はここでは上書きしない       
        mouse_rep.h       = 0;
        mouse_rep.v       = 0;
        mouse_rep.x       = y;
        mouse_rep.y       = -x;

        

        if (stat & 0x80) {
            dprintf("x:%4d y:%4d \n", mouse_rep.x,  mouse_rep.y);

            // if (state != SCROLLING) {
            //     pointing_device_set_report(mouse_rep);
            // }

            switch (state) {
                case CLICKABLE:
                    click_timer = timer_read();
                    break;

                case CLICKING:
                    after_click_lock_movement -= abs(mouse_rep.x) + abs(mouse_rep.y);

                    if (after_click_lock_movement > 0) {
                        mouse_rep.x = 0;
                        mouse_rep.y = 0;
                    }

                    break;

                case SCROLLING:
                {
                    // TODO 既定値を超えた場合のハンドリング
                    int8_t rep_v = 0;
                    int8_t rep_h = 0;
                    if (abs(mouse_rep.y) * 2 > abs(mouse_rep.x)) {

                        scroll_v_counter += mouse_rep.y;
                        while (abs(scroll_v_counter) > scroll_v_threshold) {
                            if (scroll_v_counter < 0) {
                                //tap_code16(KC_WH_U);
                                scroll_v_counter += scroll_v_threshold;
                                rep_v += scroll_v_threshold;
                            } else {
                                //tap_code16(KC_WH_D);
                                scroll_v_counter -= scroll_v_threshold;
                                rep_v -= scroll_v_threshold;
                            }
                            
                        }
                    } else {

                        scroll_h_counter += mouse_rep.x;

                        while (abs(scroll_h_counter) > scroll_h_threshold) {
                            if (scroll_h_counter < 0) {
                                // tap_code16(KC_WH_L);
                                scroll_h_counter += scroll_h_threshold;
                                rep_h += scroll_h_threshold;
                            } else {
                                // tap_code16(KC_WH_R);
                                scroll_h_counter -= scroll_h_threshold;
                                rep_h -= scroll_h_threshold;
                            }
                        }
                    }

                    mouse_rep.h = rep_h / scroll_h_threshold;
                    mouse_rep.v = -rep_v / scroll_v_threshold;
                    mouse_rep.x = 0;
                    mouse_rep.y = 0;
                }
                    break;

                case WAIT_CLICK:
                    if (timer_elapsed(click_timer) > 50) {
                        on_mouse();
                    }
                    break;

                default:
                    click_timer = timer_read();
                    state = WAIT_CLICK;
            }

            pointing_device_set_report(mouse_rep);
        }
        else
        {
            switch (state) {
                case CLICKING:
                case SCROLLING:

                    break;

                case CLICKABLE:
                    dprintf("clickable!\n");
                    if (timer_elapsed(click_timer) > 1000) {
                        off_mouse();
                    }
                    break;

                default:
                    state = NONE;
            }
        }
    }
}


/*
#include QMK_KEYBOARD_H
#include "bmp.h"
#include "bmp_custom_keycode.h"
#include "keycode_str_converter.h"
#include "pointing_device.h"
#include "paw3204.h"

report_mouse_t mouse_rep;

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    LOWER = BMP_SAFE_RANGE,
    TEST,
    RAISE,
};

const key_string_map_t custom_keys_user = {.start_kc = LOWER, .end_kc = RAISE, .key_strings = "LOWER\0TEST\0RAISE\0"};

enum layers { _BASE, _LOWER, _RAISE, _ADJUST };

const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {{{KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I, KC_J, KC_K, KC_L, KC_M, KC_N, KC_O, KC_P, KC_Q, KC_R, KC_S}}};

uint32_t keymaps_len() { return sizeof(keymaps) / sizeof(uint16_t); }

void matrix_init_user() { init_paw3204(); }

void matrix_scan_user() {
    static int  cnt;
    static bool paw_ready;
    if (cnt++ % 50 == 0) {
        uint8_t pid = read_pid_paw3204();
        if (pid == 0x30) {
            dprint("paw3204 OK\n");
            paw_ready = true;
        } else {
            dprintf("paw3204 NG:%d\n", pid);
            paw_ready = false;
        }
    }

    if (paw_ready) {
        uint8_t stat;
        int8_t x, y;

        read_paw3204(&stat, &x, &y);
        mouse_rep.buttons = 0;
        mouse_rep.h       = 0;
        mouse_rep.v       = 0;
        mouse_rep.x       = y;
        mouse_rep.y       = -x;

        dprintf("stat:0x%02x x:%4d y:%4d\n", stat, mouse_rep.x, mouse_rep.y);

        if (stat & 0x80) {
            pointing_device_set_report(mouse_rep);
        }
    }
}

bool process_record_user(uint16_t keycode, keyrecord_t* record) {
    // bool continue_process = process_record_user_bmp(keycode, record);
    // if (continue_process == false) {
    //     return false;
    // }
    SEND_STRING("gey");
    dprintf("pre");
    switch (keycode) {
        case LOWER:
            if (record->event.pressed) {
                layer_on(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            } else {
                layer_off(_LOWER);
                update_tri_layer(_LOWER, _RAISE, _ADJUST);
            }
            return false;
            break;
        case RAISE:
            SEND_STRING("QMK is awesome2.");
            return false;
            break;
        case TEST:
            SEND_STRING("QMK is awesome.");
            return false;
            break;
        default:
            break;
    }

    return true;
}
*/

/*
#include QMK_KEYBOARD_H
#include "bmp.h"
#include "bmp_custom_keycode.h"
#include "keycode_str_converter.h"

// Defines the keycodes used by our macros in process_record_user
enum custom_keycodes {
    LOWER = BMP_SAFE_RANGE,
    RAISE,
};

const key_string_map_t custom_keys_user =
{
    .start_kc = LOWER,
    .end_kc = RAISE,
    .key_strings = "LOWER\0RAISE\0"
};

enum layers {
    _BASE, _LOWER, _RAISE, _ADJUST
};

const uint16_t keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    {{
    KC_A, KC_B, KC_C, KC_D, KC_E, KC_F, KC_G, KC_H, KC_I,
        KC_J, KC_K, KC_L, KC_M, KC_N, KC_O, KC_P, KC_Q, KC_R, KC_S
    }}
};

uint32_t keymaps_len() {
  return 19;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  bool continue_process = process_record_user_bmp(keycode, record);
  if (continue_process == false)
  {
    return false;
  }

  switch (keycode) {
    case LOWER:
      if (record->event.pressed) {
        layer_on(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_LOWER);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    case RAISE:
      if (record->event.pressed) {
        layer_on(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      } else {
        layer_off(_RAISE);
        update_tri_layer(_LOWER, _RAISE, _ADJUST);
      }
      return false;
      break;
    default:
      break;
  }

  return true;
}

*/