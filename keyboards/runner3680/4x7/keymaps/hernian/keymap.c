#include QMK_KEYBOARD_H
#include "usjp.h"

#ifdef RGBLIGHT_ENABLE
//Following line allows macro to read current RGB settings
extern rgblight_config_t rgblight_config;
rgblight_config_t RGB_current_config;
#endif

enum layer_number {
    LAYER_COLMAK = 0,
    LAYER_LOWER = 1,
    LAYER_RAISE = 2,
    LAYER_IMM = 3,
};


#define KC_LOWR MO(LAYER_LOWER)
#define KC_CLWR LM(LAYER_LOWER, MOD_LCTL)
#define KC_RAIS MO(LAYER_RAISE)
#define KC_IMM  MO(LAYER_IMM)
#define KC_ZNHN 0x35
#define KC_EISU 0x39
#define KC_HIRA 0x88
#define KC_XFER 0x8a
#define KC_NFER 0x8b


// Fillers to make layering more clear
#define EISU LALT(KC_GRV)


enum custom_keycode {
	KC_MLCK = SAFE_RANGE,
	KC_CFHR = LCTL(KC_U),
	KC_CFKT = LCTL(KC_I),
	KC_CFES = LCTL(KC_P),
	KC_CHES = LCTL(KC_T)
};


const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {

    /* QWERTY
     * ,------------------------------------------------.      ,------------------------------------------------.
     * | ERC  |   Q  |   W  |   F  |   P  |   G  |      |      |      |   J  |   L  |   U  |   Y  |   :  |   -  |
     * |------+------+------+------+------+------+------|      |------+------+------+------+------+------+------|
     * | Tab  |   A  |   R  |   S  |   T  |   D  |      |      |      |   H  |   N  |   E  |   I  |   O  |   '  |
     * |------+------+------+------+------+------+------|      |------+------+------+------+------+------+------|
     * | Shift|   Z  |   X  |   C  |   V  |   B  |      |      |      |   K  |   M  |   ,  |   .  |   /  | Enter|
     * |------+------+------+------+------+------+------|      |------+------+------+------+------+------+------|
     * | Alt  | Ctrl |  Gui |  Alt | LOWR | Shift| Ctrl |      | Raise| Space| Nfer |  Alt |  Gui |  Crl |  Alt |
     * `------------------------------------------------'      `------------------------------------------------'
     */
  [LAYER_COLMAK] = LAYOUT(
      KC_ESC,  KC_Q,    KC_W,    KC_F,    KC_P,    KC_G,    KC_NO,      KC_NO,   KC_J,    KC_L,    KC_U,    KC_Y,    KC_SCLN, KC_MINS,
      KC_TAB,  KC_A,    KC_R,    KC_S,    KC_T,    KC_D,    KC_NO,      KC_NO,   KC_H,    KC_N,    KC_E,    KC_I,    KC_O,    KC_QUOT,
      KC_BSPC, KC_Z,    KC_X,    KC_C,    KC_V,    KC_B,    KC_NO,      KC_NO,   KC_K,    KC_M,    KC_COMM, KC_DOT,  KC_SLSH, KC_ENT,
      KC_NO,   KC_MLCK, KC_LGUI, KC_LALT, KC_LOWR, KC_LSFT, KC_LCTL,    KC_RAIS, KC_SPC,  KC_NFER, KC_RCTL, KC_RGUI, KC_MLCK, KC_NO
      ),

  [LAYER_LOWER] =  LAYOUT(
      KC_GRV,  KC_EXLM, KC_AT,   KC_HASH, KC_DLR,  KC_PERC, KC_NO,      KC_NO,   KC_CIRC, KC_AMPR, KC_ASTR, KC_LPRN, KC_RPRN, KC_PIPE,
      KC_TILD, KC_1,    KC_2,    KC_3,    KC_4,    KC_5,    KC_NO,      KC_NO,   KC_6,    KC_7,    KC_8,    KC_9,    KC_0,    KC_BSLS,
      KC_DEL,  KC_LBRC, KC_RBRC, KC_LCBR, KC_RCBR, KC_MINS, KC_NO,      KC_NO,   KC_PLUS, KC_EQL,  KC_COMM, KC_DOT,  KC_SLSH, KC_NO,
      KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS
      ),

  [LAYER_RAISE] =  LAYOUT(
      KC_CAPS, KC_F1,   KC_F2,   KC_F3,   KC_F4,   KC_F5,   KC_NO,      KC_NO,   KC_INS,  KC_NO,   KC_BSPC, KC_DEL,  KC_NO,   KC_PSCR,
      KC_NO,   KC_F6,   KC_F7,   KC_F8,   KC_F9,   KC_F10,  KC_NO,      KC_NO,   KC_LEFT, KC_DOWN, KC_UP,   KC_RGHT, KC_NO,   KC_NO,
      KC_NO,   KC_F11,  KC_F12,  KC_PAUS, KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_HOME, KC_PGDN, KC_PGUP, KC_END,  KC_NO,   KC_NO,
      KC_NO,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO
      ),

  [LAYER_IMM] =  LAYOUT(
      KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_NO,   KC_ZNHN, KC_HIRA, KC_EISU, KC_NO,   KC_NO,
      KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_NO,   KC_NO,   KC_NFER, KC_XFER, KC_NO,   KC_NO,
      KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,   KC_NO,      KC_NO,   KC_NO,   KC_CFHR, KC_CFKT, KC_CFES, KC_CHES, KC_NO,
      KC_NO,   KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS,    KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_TRNS, KC_NO
      )
};


bool is_mod_lock = false;
uint16_t locked_mods = 0x00;
uint16_t keycode_prev = 0x00;


bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    bool res = process_usjp(keycode, record);
    if (!res){
        return false;
    }
    bool is_tapped = !record->event.pressed && (keycode == keycode_prev);
    keycode_prev = keycode;

    if (keycode ==KC_MLCK) {
        is_mod_lock = record->event.pressed;
        if (locked_mods != 0x00){
            unregister_mods(locked_mods);
            locked_mods = 0x00;
        }
        layer_off(LAYER_LOWER);
        layer_off(LAYER_RAISE);
        return false;
    }
    if (IS_MOD(keycode)) {
        if (is_mod_lock) {
            if (record->event.pressed) {
                locked_mods |= MOD_BIT(keycode);
                register_mods(locked_mods);
            }
            return false;
        }
        return true;
    }
    if (keycode == KC_LOWR){
        if (is_mod_lock) {
            if (record->event.pressed) {
                layer_on(LAYER_LOWER);
            }
            return false;
        }
        return true;
    }
    if (keycode == KC_RAIS) {
        if (is_mod_lock) {
            if (record->event.pressed) {
                layer_on(LAYER_RAISE);
            }
            return false;
        }
        return true;
    }
    if (keycode == KC_NFER) {
        if (record->event.pressed) {
            layer_on(LAYER_IMM);
        } else {
            layer_off(LAYER_IMM);
        }
        if (is_tapped) {
            tap_code(KC_NFER);
        }
        return false;
    }
    return true;
}

void post_process_record_user(uint16_t keycode, keyrecord_t *record) {
    post_process_usjp(keycode, record);
}

