#include "keyboard.h"
#include "util/mem.h"
#include "util/port.h"
#include "util/types.h"
#include "util/debug.h"
#include "tty/tty.h"

static bool k_shift = false;
static bool k_ctrl = false;
static bool k_alt = false;

static char scancode_map[128];
static char scancode_map_shift[128];
static char scancode_map_alt[128];

void keyboard_init() {
    set_isr_function(33, keyboard_handler);
}

void keyboard_handler(InterruptFrame* frame) {
    (void)frame;

    char scan_code = inb(0x60) & 0x7F;
    char press = inb(0x60) & 0x80;

    (void)scan_code;
    (void)press;

    if (scan_code == 29) {
        k_ctrl = !press;
    } else if (scan_code == 42) {
        k_shift = !press;
    } else if (scan_code == 56) {
        k_alt = !press;
    }


    char c;

    if (k_shift) {
        c = scancode_map_shift[(u32)scan_code];
    } else if (k_alt) {
        c = scancode_map_alt[(u32)scan_code];
    } else {
        c = scancode_map[(u32)scan_code];
    }

    if (!press && c != 0) {
        tty_color(80, 0x03);
        tty_putchar(c);
    }

    /*if (!press) {*/
        /*static char buf[32];*/
        /*itoa(buf, scan_code, 32, 10);*/

        /*tty_color(80, 0x03);*/
        /*tty_puts(buf);*/
        /*tty_puts("\n");*/
    /*}*/
}

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