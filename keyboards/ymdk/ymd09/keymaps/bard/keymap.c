#include QMK_KEYBOARD_H

//#define TESTING

#include "../../../../macro.h"

enum my_keycodes {
  TWIST2 = SAFE_RANGE,
  TWIST3,
  TWIST4,
  TWIST_MANA,
  POP5,
  POP6,
  POP7,
  POP8,
  A1,
  A3,
  A5,
  T2,
  T4,
  T6,
  TRAINING1,
  TRAINING2,
  END_TWIST
};

enum layer_names {
    _BARD_GRP,
    _BARD_EXP,
    _TRAINING,
    _PAD,
    _LAYER_CHOOSE,
};

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
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
    case A1:
      if (record->event.pressed) {
        start_pop(&m_sevenOne);
      }
      return false;
    case A3:
      if (record->event.pressed) {
        start_pop(&m_sevenThree);
      }
      return false;
    case A5:
      if (record->event.pressed) {
        start_pop(&m_sevenFive);
      }
      return false;
    case T2:
      if (record->event.pressed) {
        start_pop(&m_eightTwo);
      }
      return false;
    case T4:
      if (record->event.pressed) {
        start_pop(&m_eightFour);
      }
      return false;
    case T6:
      if (record->event.pressed) {
        start_pop(&m_eightSix);
      }
      return false;
    case TRAINING1:
      if (record->event.pressed) {
        start_twist(&training1, record, false);
      }
      return false;
    case TRAINING2:
      if (record->event.pressed) {
        start_twist(&training2, record, false);
      }
      return false;
    default:
      return true; // Process all other keycodes normally
  }
}

void keyboard_post_init_user(void) {
  // Set all keys to off.
  rgb_matrix_mode_noeeprom(RGB_MATRIX_SOLID_COLOR);
  rgb_matrix_sethsv_noeeprom(HSV_OFF);

  macro_post_init_user();
}

bool rgb_matrix_indicators_user(void) {
  // Light up choose buttons.
  if (IS_LAYER_ON(_LAYER_CHOOSE)) {
    rgb_matrix_set_color(g_led_config.matrix_co[0][2], 0, 0, 255);
    rgb_matrix_set_color(g_led_config.matrix_co[0][0], 0, 0, 255);
    return true;
  }

  // Subtle light up the layer indicator.
  if (IS_LAYER_ON(_BARD_EXP)) {
    rgb_matrix_set_color(g_led_config.matrix_co[0][2], 0, 0, 15);
  } else if (IS_LAYER_ON(_TRAINING)) {
    rgb_matrix_set_color(g_led_config.matrix_co[1][2], 0, 0, 15);
  } else if (IS_LAYER_ON(_PAD)) {
    rgb_matrix_set_color(g_led_config.matrix_co[1][0], 0, 0, 15);
  } else {
    rgb_matrix_set_color(g_led_config.matrix_co[0][0], 0, 0, 15);
  }

  if (m_twist) {
    // Light up the current twist key.
    rgb_matrix_set_color(g_led_config.matrix_co[twist_keypos.row][twist_keypos.col], 0, 255, 0);
    // Light up the stop song key.
    rgb_matrix_set_color(g_led_config.matrix_co[0][0], 255, 0, 0);
  }
  return true;
}

void matrix_scan_user(void) {
  int macros_len = 2;
  Macro* macros[] = { m_twist, m_pop };

  for (int i=0; i < macros_len; i++) {
    if (macros[i]) {
      step_macro(macros[i]);
    }
  }
}


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

  [_BARD_GRP] = LAYOUT(END_TWIST, LT(_LAYER_CHOOSE, KC_KP_8), TWIST2,
                       TWIST3, TWIST4, TWIST_MANA, 
                       POP7, POP8, KC_1),

  [_BARD_EXP] = LAYOUT(END_TWIST, _______, TWIST2,
                       TWIST3, POP6, KC_7, 
                       KC_Q, POP5, KC_1),

  [_TRAINING] = LAYOUT(END_TWIST, _______, KC_9,
                       KC_4, KC_5, KC_6, 
                       TRAINING1, TRAINING2, KC_3),

  [_PAD] = LAYOUT(END_TWIST, _______, A5,
                  T6, T4, A3,
                  KC_Q, T2, A1),

  [_LAYER_CHOOSE] = LAYOUT(TO(_BARD_GRP), _______, TO(_BARD_EXP),
                           TO(_PAD), _______, TO(_TRAINING),
                           _______, _______, _______),
};
