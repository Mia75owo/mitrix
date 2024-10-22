#include "keyboard.h"

#include "gfx/gui.h"
#include "util/debug.h"
#include "util/port.h"
#include "util/types.h"
#include "events/events.h"

static bool k_shift = false;
static bool k_ctrl = false;
static bool k_alt = false;

static char scancode_map[128];
static char scancode_map_shift[128];
static char scancode_map_alt[128];

static bool extended = false;

void keyboard_init() { set_isr_function(33, keyboard_handler); }

void keyboard_handler(CPUState* frame) {
    (void)frame;

    KeyEvent event;
    event.special = false;

    u8 data = inb(0x60);
    u8 scan_code = data & 0x7F;
    u8 pressed = !(data & 0x80);

    // Some keys get send in two steps
    if (scan_code == 96) {
        extended = true;
        return;
    }

    if (scan_code == 29) {
        k_ctrl = pressed;
        event.special = true;
    } else if (scan_code == 42) {
        k_shift = pressed;
        event.special = true;
    } else if (scan_code == 56) {
        k_alt = pressed;
        event.special = true;
    }

    // "parse" the key
    if (k_shift) {
        event.c = scancode_map_shift[(u32)scan_code];
    } else if (k_alt) {
        event.c = scancode_map_alt[(u32)scan_code];
    } else {
        event.c = scancode_map[(u32)scan_code];
    }

    if (scan_code == 1) {
        event.c = KEYCODE_ESCAPE;
        event.special = true;
    } else if (scan_code >= 59 && scan_code <= 59 + 10) {
        event.c = KEYCODE_F1 + (scan_code - 59);
        event.special = true;
    } else if (scan_code == 87) {
        event.c = KEYCODE_F11;
        event.special = true;
    } else if (scan_code == 88) {
        event.c = KEYCODE_F12;
        event.special = true;
    }

    if (extended) {
        if (scan_code == 72) {
            event.c = KEYCODE_UP;
            event.special = true;
        } else if (scan_code == 75) {
            event.c = KEYCODE_LEFT;
            event.special = true;
        } else if (scan_code == 80) {
            event.c = KEYCODE_DOWN;
            event.special = true;
        } else if (scan_code == 77) {
            event.c = KEYCODE_RIGHT;
            event.special = true;
        } else if (scan_code == 91) {
            event.c = KEYCODE_MOD;
            event.special = true;
        }
        extended = false;
    }

    event.pressed = pressed;
    event.shift = k_shift;
    event.ctrl = k_ctrl;
    event.alt = k_alt;
    event.extended = extended;
    event.raw = scan_code;

    Task* focused_task = events_get_focused_task();
    if (focused_task == NULL || focused_task->is_kernel_task) {
        gui_key_event(event);
    }
    events_key_event(event);
}

// clang-format off
static char scancode_map[128] = {
    0,        27,/*esc*/'1',      '2',      '3',      '4',      '5',      '6',
    '7',      '8',      '9',      '0',      '.',/*ß*/ '.',/*´*/ '\b',     '\t',
    'q',      'w',      'e',      'r',      't',      'z',      'u',      'i',
    'o',      'p',      '.',/*ü*/ '+',      '\n',     0,/*ctrl*/'a',      's',
    'd',      'f',      'g',      'h',      'j',      'k',      'l',      '.',/*ö*/
    '.',/*ä*/ '.',/*?*/ 0,/*shft*/0,/*past*/'y',      'x',      'c',      'v',
    'b',      'n',      'm',      ',',      '.',      '-',      0,/*shft*/'.',/*?*/
    0,/*alt*/ ' ',      0,/*?*/   0,/*f1*/  0,/*f2*/  0,/*f3*/  0,/*f4*/  0,/*f5*/
    0,/*f6*/  0,/*f7*/  0,/*f8*/  0,/*f9*/  0,/*f10*/ 0,/*numl*/0,/*scrl*/'7',
    '8',      '9',      '-',      '4',      '5',      '6',      '+',      '1',
    '2',      '3',      '0',      ',',      '.',/*?*/ '.',/*?*/ '<',      0,/*f11*/
    0,/*f12*/ 0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
//  .         .         .         .         .         .         .         .
};

static char scancode_map_shift[128] = {
    0,        27,/*esc*/'!',      '"',      '.',/*§*/ '$',      '%',      '&',
    '/',      '(',      ')',      '=',      '?',      '`',      '\b',     '\t',
    'Q',      'W',      'E',      'R',      'T',      'Z',      'U',      'I',
    'O',      'P',      '.',/*Ü*/ '*',      '\n',     0,/*ctrl*/'A',      'S',
    'D',      'F',      'G',      'H',      'J',      'K',      'L',      '.',/*Ö*/
    '.',/*Ä*/ '.',/*?*/ 0,/*shft*/0,/*past*/'Y',      'X',      'C',      'V',
    'B',      'N',      'M',      ';',      ':',      '_',      0,/*shft*/'.',/*?*/
    0,/*alt*/ ' ',      0,/*?*/   0,/*f1*/  0,/*f2*/  0,/*f3*/  0,/*f4*/  0,/*f5*/
    0,/*f6*/  0,/*f7*/  0,/*f8*/  0,/*f9*/  0,/*f10*/ 0,/*numl*/0,/*scrl*/'7',
    '8',      '9',      '-',      '4',      '5',      '6',      '+',      '1',
    '2',      '3',      '0',      ',',      '.',/*?*/ '.',/*?*/ '>',      0,/*f11*/
    0,/*f12*/ 0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
//  .         .         .         .         .         .         .         .
};

static char scancode_map_alt[128] = {
    0,        27,/*esc*/'1',      '2',      '3',      '4',      '5',      '6',
    '{',      '[',      ']',      '}',      '\\',     '.',/*¸*/ '\b',     '\t',
    '@',      'w',      'e',      'r',      't',      'z',      'u',      'i',
    'o',      'p',      '.',/*ü*/ '~',      '\n',     0,/*ctrl*/'a',      's',
    'd',      'f',      'g',      'h',      'j',      'k',      'l',      '.',/*ö*/
    '.',/*ä*/ '.',/*?*/ 0,/*shft*/0,/*past*/'y',      'x',      'c',      'v',
    'b',      'n',      'm',      ',',      '.',      '-',      0,/*shft*/'.',/*?*/
    0,/*alt*/ ' ',      0,/*?*/   0,/*f1*/  0,/*f2*/  0,/*f3*/  0,/*f4*/  0,/*f5*/
    0,/*f6*/  0,/*f7*/  0,/*f8*/  0,/*f9*/  0,/*f10*/ 0,/*numl*/0,/*scrl*/'7',
    '8',      '9',      '-',      '4',      '5',      '6',      '+',      '1',
    '2',      '3',      '0',      ',',      '.',/*?*/ '.',/*?*/ '|',      0,/*f11*/
    0,/*f12*/ 0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
    0,        0,        0,        0,        0,        0,        0,        0,
//  .         .         .         .         .         .         .         .
};
// clang-format on
