/* Copyright 2025 @ Keychron (https://www.keychron.com)
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
#include "keychron_common.h"

//#define TESTING

#include <macro.h>

enum layers{
  WIN_BASE,
  WIN_FN,
  _BARD_GRP,
};

enum my_keycodes {
  TWIST2 = SAFE_RANGE,
  TWIST3,
  TWIST4,
  TWIST_MANA,
  HOLD2,
  HOLD3,
  AUTO2,
  POP5,
  POP6,
  POP7,
  POP8,
  GGR_ROOT,
  TRAINING1,
  TRAINING2,
  END_TWIST
};

// 30 min to reset to base layer.
#define LAYER_TIMEOUT 1800000
static uint32_t key_timer; 

HoldRepeat* s_holds[] = { &m_h2, &m_h3, 0 };

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
  key_timer = timer_read32(); // store time of last refresh

  switch (keycode) {
    case TWIST2:
      if (record->event.pressed) {
        start_twist(&m_twist2, record, true);
      }
      return false;
    case TWIST3:
      if (record->event.pressed) {
        start_twist(&m_twist3, record, true);
      }
      return false;
    case TWIST4:
      if (record->event.pressed) {
        start_twist(&m_twist4, record, true);
      }
      return false;
    case TWIST_MANA:
      if (record->event.pressed) {
        start_twist(&m_twist_mana, record, true);
      }
      return false;
    case HOLD2:
      if (record->event.pressed) {
        start_hold_repeat(&m_h2, record);
      } else {
        release_hold_repeat(&m_h2);
      }
      return false;
    case HOLD3:
      if (record->event.pressed) {
        start_hold_repeat(&m_h3, record);
      } else {
        release_hold_repeat(&m_h3);
      }
      return false;
    case AUTO2:
      if (record->event.pressed) {
        start_twist(&m_auto2, record, false);
      }
      return false;
    case END_TWIST:
      if (record->event.pressed) {
        stop_twist();
      }
      return false;
    case POP5:
      if (record->event.pressed) {
        start_pop(&m_pop5);
      }
      return false;
    case POP6:
      if (record->event.pressed) {
        start_pop(&m_pop6);
      }
      return false;
    case POP7:
      if (record->event.pressed) {
        start_pop(&m_pop7);
      }
      return false;
    case POP8:
      if (record->event.pressed) {
        start_pop(&m_pop8);
      }
      return false;
    case GGR_ROOT:
      if (record->event.pressed) {
        start_macro(&m_charm_break_root);
      }
      return false;
    default:
      return true; // Process all other keycodes normally
  }
  if (!process_record_keychron_common(keycode, record)) {
      return false;
  }
  return true;
}

void keyboard_post_init_user(void) {
  // Set all keys to off.
  //rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  //rgb_matrix_sethsv_noeeprom(HSV_OFF);

  macro_post_init_user();

  key_timer = timer_read32(); // Start key timer.
}

bool rgb_matrix_indicators_user(void) {
  // Light up choose buttons.
  if (IS_LAYER_ON(WIN_FN)) {
    rgb_matrix_set_color(14, 0, 0, 255);
    rgb_matrix_set_color(15, 0, 0, 255);
    //rgb_matrix_set_color(16, 0, 0, 255);
    return true;
  }

  // Clear the backlight on the mode keys.
  rgb_matrix_set_color(14, 0, 0, 0);
  rgb_matrix_set_color(15, 0, 0, 0);
  rgb_matrix_set_color(16, 0, 0, 0);
  rgb_matrix_set_color(17, 0, 0, 0);

  // Subtle light up the layer indicator.
  if (IS_LAYER_ON(_BARD_GRP)) {
    rgb_matrix_set_color(15, 0, 0, 255);
  //} else if (IS_LAYER_ON(_ENC_GRP)) {
  //  rgb_matrix_set_color(16, 0, 0, 255);
  } else {
    rgb_matrix_set_color(14, 0, 0, 255);
  }

  if (m_twist) {
    // Light up the current twist key.
    rgb_matrix_set_color(g_led_config.matrix_co[twist_keypos.row][twist_keypos.col], 255, 255, 255);
    // Light up the stop song key.
    rgb_matrix_set_color(17, 255, 0, 0);
  }
  for (int i=0; s_holds[i]; i++) {
    if (s_holds[i]->active) {
      // Light up the hold key.
      rgb_matrix_set_color(g_led_config.matrix_co[s_holds[i]->keypos.row][s_holds[i]->keypos.col], 255, 255, 255);
    }
  }
  return true;
}

void matrix_scan_user(void) {
  static int macros_len = 3;
  Macro* macros[] = { &m_charm_break_root, m_twist, m_pop };

  for (int i=0; i < macros_len; i++) {
    if (macros[i]) {
      step_macro(macros[i]);
    }
  }

  for (int i=0; s_holds[i]; i++) {
    step_hold_repeat(s_holds[i]);
  }
}

void housekeeping_task_user(void) {
    if (!IS_LAYER_ON(WIN_BASE) && timer_elapsed32(key_timer) > LAYER_TIMEOUT)
    {
        layer_move(WIN_BASE);
    }
}


// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [WIN_BASE] = LAYOUT_108_ansi(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,   KC_F12,   KC_PSCR,  KC_CTANA, RGB_MOD,  KC_F13,   KC_F14,   KC_F15,   KC_F16,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,   KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,  KC_NUM,   KC_PSLS,  KC_PAST,  KC_PMNS,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,  KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,  KC_P7,    KC_P8,    KC_P9,    KC_PPLS,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,            KC_ENT,                                 KC_P4,    KC_P5,    KC_P6,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,            KC_RSFT,            KC_UP,              KC_P1,    KC_P2,    KC_P3,    KC_PENT,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN, MO(WIN_FN),KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT,  KC_P0,              KC_PDOT          ),

    [WIN_FN] = LAYOUT_108_ansi(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FILE,  RGB_VAD,  RGB_VAI,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,  KC_VOLU,  _______,  _______,  RGB_TOG, TO(WIN_BASE), TO(_BARD_GRP), _______, _______,
        _______,  BT_HST1,  BT_HST2,  BT_HST3,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        RGB_TOG,  RGB_MOD,  RGB_VAI,  RGB_HUI,  RGB_SAI,  RGB_SPI,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  RGB_RMOD, RGB_VAD,  RGB_HUD,  RGB_SAD,  RGB_SPD,  _______,  _______,  _______,  _______,  _______,  _______,            _______,                                _______,  _______,  _______,
        _______,            _______,  _______,  _______,  _______,  BAT_LVL,  _______,  _______,  _______,  _______,  _______,            _______,            _______,            _______,  _______,  _______,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______          ),

    [_BARD_GRP] = LAYOUT_108_ansi(
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  END_TWIST,
        _______,  _______,  HOLD2,    HOLD3,    _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,
        _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,                                TWIST2,   _______,  _______,
        _______,            _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,            _______,            _______,            TWIST3,   TWIST4,   TWIST_MANA,  _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,  _______,  _______,  _______,  _______,  AUTO2,              POP8             )
};
// clang-format on

