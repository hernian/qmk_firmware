/// US配列風のキーマップでJP配列のPCへ接続出来るようにする
#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#if defined(__AVR__)
#    include <avr/pgmspace.h>
#endif
#include "action.h"
#include "action_util.h"
#include "keycode.h"
#include "quantum_keycodes.h"
#include "print.h"
#include "usjp.h"


#define KC_JP_1     0x1e
#define KC_JP_2     0x1f
#define KC_JP_6     0x23
#define KC_JP_7     0x24
#define KC_JP_8     0x25
#define KC_JP_9     0x26
#define KC_JP_0     0x27
#define KC_JP_MINS  0x2d
#define KC_JP_CRT   0x2e
#define KC_JP_AT    0x2f
#define KC_JP_LBRC  0x30
#define KC_JP_RBRC  0x32
#define KC_JP_SCLN  0x33
#define KC_JP_CLN   0x34
#define KC_JP_BSLS  0x87
#define KC_JP_YEN   0x89

#define NUM_ELEMENTS(a) (sizeof(a) / sizeof((a)[0]))

typedef enum  {
    SHIFT_MODE_NORMAL = 0,
    SHIFT_MODE_DOWN = 1,
    SHIFT_MODE_UP = 2
} shift_mode_t;


typedef struct {
    uint8_t _kc_src;
    uint8_t _shift_src;
    uint8_t _kc_dst;
    uint8_t _shift_dst;
} usjp_desc_t;

inline uint8_t get_kc_src(const usjp_desc_t* desc) {
    return pgm_read_byte(&desc->_kc_src);
}

inline uint8_t get_kc_dst(const usjp_desc_t* desc) {
    return pgm_read_byte(&desc->_kc_dst);
}

inline bool get_shift_src(const usjp_desc_t* desc) {
    return (bool)pgm_read_byte(&desc->_shift_src);
}

inline bool get_shift_dst(const usjp_desc_t* desc) {
    return (bool)pgm_read_byte(&desc->_shift_dst);
}

typedef struct {
    keypos_t    key;
    uint8_t     kc_dst;
} key_down_t;


static const usjp_desc_t USJP_DESC_TABLE[] PROGMEM = {
    {KC_2,      true,   KC_JP_AT,     false},
    {KC_6,      true,   KC_JP_CRT,    false},
    {KC_7,      true,   KC_JP_6,      true},
    {KC_8,      true,   KC_JP_CLN,    true},
    {KC_9,      true,   KC_JP_8,      true},
    {KC_0,      true,   KC_JP_9,      true},
    {KC_MINS,   true,   KC_JP_BSLS,   true},
    {KC_SCLN,   true,   KC_JP_CLN,    false},
    {KC_BSLS,   false,  KC_JP_YEN,    false},
    {KC_BSLS,   true,   KC_JP_YEN,    true},
    {KC_GRV,    false,  KC_JP_AT,     true},
    {KC_GRV,    true,   KC_JP_CRT,    true},
    {KC_LBRC,   false,  KC_JP_LBRC,   false},
    {KC_LBRC,   true,   KC_JP_LBRC,   true},
    {KC_RBRC,   false,  KC_JP_RBRC,   false},
    {KC_RBRC,   true,   KC_JP_RBRC,   true},
    {KC_EQL,    false,  KC_JP_MINS,   true},
    {KC_EQL,    true,   KC_JP_SCLN,   true},
    {KC_QUOT,   true,   KC_JP_2,      true},
    {KC_QUOT,   false,  KC_JP_7,      true}
};
#define USJP_DESC_COUNT NUM_ELEMENTS(USJP_DESC_TABLE)


static bool save_flag;
static uint8_t  save_shifts;

#define KEY_DOWN_COUNT_MAX 6
static int key_down_count = 0;
static key_down_t KEY_DOWN_TABLE[KEY_DOWN_COUNT_MAX];

#ifdef CONSOLE_ENABLE
#   define DEBUG_PRINT(...)    uprintf(__VA_ARGS__);
void REGISTER_CODE(uint8_t code) {
    uprintf("RC: %02X\n", code);
    register_code(code);
}
void UNREGISTER_CODE(uint8_t code) {
    uprintf("UC: %02X\n", code);
    unregister_code(code);
}
void REGISTER_MODS(uint8_t mods) {
    uprintf("RM: %02X\n", mods);
    register_mods(mods);
}
void UNREGISTER_MODS(uint8_t mods) {
    uprintf("UM: %02X\n", mods);
    unregister_mods(mods);
}
#else
#   define DEBUG_PRINT(...)
#   define REGISTER_CODE    register_code
#   define UNREGISTER_CODE  unregister_code
#   define REGISTER_MODS    register_mods
#   define UNREGISTER_MODS  unregister_mods
#endif


static const usjp_desc_t* find_desc(uint16_t keycode, bool shift) {
    uint16_t qk_code = (keycode & 0xff00);
    if ((qk_code == 0x0000) || (qk_code == QK_LSFT) || (qk_code == QK_RSFT)) {
        if ((qk_code == QK_LSFT) || (qk_code == QK_RSFT)) {
            shift = true;
        }
        uint8_t kc_src = (uint8_t)(keycode & 0x00ff);
        // DEBUG_PRINT("find_desc: %04X, %b, %04X\n", kc_src, shift, qk_code)
        for (int idx = 0; idx < USJP_DESC_COUNT; ++idx) {
            const usjp_desc_t* desc = &USJP_DESC_TABLE[idx];
            uint8_t desc_kc_src = get_kc_src(desc);
            bool desc_shift_src = get_shift_src(desc);
            if (kc_src == desc_kc_src && shift == desc_shift_src) {
                return desc;
            }
        }
    }
    return NULL;
}

static void set_shift(bool shift) {
    save_shifts = get_mods() & MOD_MASK_SHIFT;
    if (shift) {
        if (save_shifts == 0x00) {
            save_flag = true;
            REGISTER_MODS(MOD_BIT(KC_LSFT));
        }
    } else {
        if (save_shifts != 0x00) {
            save_flag = true;
            UNREGISTER_MODS(MOD_MASK_SHIFT);
        }
    }
}

static void restore_shift(void) {
    if (save_flag) {
        save_flag = false;
        uint8_t mods_shift_now = get_mods() & MOD_MASK_SHIFT;
        if (mods_shift_now != save_shifts) {
            if (save_shifts != 0x00) {
                REGISTER_MODS(save_shifts);
            }
            uint8_t clear_shifts = ~save_shifts & MOD_MASK_SHIFT;
            if (clear_shifts != 0x00) {
                UNREGISTER_MODS(clear_shifts);
            }
        }
    }
}

#ifdef CONSOLE_ENABLE
static void print_down_table(void) {
    uprintf("DT: %d ", key_down_count);
    for (int idx = 0; idx < key_down_count; ++idx) {
        key_down_t* key_down = &KEY_DOWN_TABLE[idx];
        uprintf("[%d, %d, %02X]", key_down->key.col, key_down->key.row, key_down->kc_dst);
    }
    uprintf("\n");
}
#endif

static void add_down_key(keypos_t key, uint8_t kc_dst) {
    if (key_down_count >= KEY_DOWN_COUNT_MAX) {
        UNREGISTER_CODE(KEY_DOWN_TABLE[0].kc_dst);
        restore_shift();
        --key_down_count;
        size_t cb_move = sizeof(key_down_t) * key_down_count;
        if (cb_move > 0) {
            memmove(&KEY_DOWN_TABLE[0], &KEY_DOWN_TABLE[1], cb_move);
        }
    }
    key_down_t* key_down = &KEY_DOWN_TABLE[key_down_count];
    key_down->key = key;
    key_down->kc_dst = kc_dst;
    ++key_down_count;
#ifdef CONSOLE_ENABLE
    uprintf("ADK ");
    print_down_table();
#endif
}

static uint8_t del_down_key(keypos_t key) {
    uint8_t res = 0x00;
    for (int idx = 0; idx < key_down_count; ++idx) {
        key_down_t* key_down = &KEY_DOWN_TABLE[idx];
        if (memcmp(&key_down->key, &key, sizeof(keypos_t)) == 0) {
            --key_down_count;
            res = key_down->kc_dst;
            size_t cb_move = sizeof(key_down_t) * (key_down_count - idx);
            if (cb_move > 0){
                memmove(key_down, key_down + 1, cb_move);
            }
            break;
        }
    }
#ifdef CONSOLE_ENABLE
    uprintf("DDK ret: %02X ", res);
    print_down_table();
#endif
    return res;
}

bool process_usjp(uint16_t keycode, keyrecord_t* record) {
#ifdef CONSOLE_ENABLE
    {
        uint8_t mods = get_mods();
        uprintf("process_usjp: %04X, %b, %02X\n", keycode, record->event.pressed, mods);
    }
#endif
    if (record->event.pressed) {
        restore_shift();
        uint8_t mods = get_mods();
        bool shift = (mods & MOD_MASK_SHIFT) != 0;
        const usjp_desc_t* desc = find_desc(keycode, shift);
        if (desc == NULL) {
            return true;
        }
        uint8_t desc_kc_dst = get_kc_dst(desc);
        bool shift_dst = get_shift_dst(desc);
        DEBUG_PRINT("USJP: %04X, %b -> %02X, %b\n", keycode, shift, desc_kc_dst, shift_dst)
        add_down_key(record->event.key, desc_kc_dst);
        set_shift(shift_dst);
        REGISTER_CODE(desc_kc_dst);
        return false;
    }
    uint8_t kc_dst = del_down_key(record->event.key);
    if (kc_dst != 0x00) {
        UNREGISTER_CODE(kc_dst);
        restore_shift();
        return false;
    }
    return true;
}

void post_process_usjp(uint16_t keycode, keyrecord_t* record) {
#ifdef CONSOLE_ENABLE
    uint8_t mods = get_mods();
    uprintf("post_process_usjp: %04X, %b, %02X\n", keycode, record->event.pressed, mods);
#endif
}

void clear_usjp(void) {
    DEBUG_PRINT("clear_usjp\n")
}
