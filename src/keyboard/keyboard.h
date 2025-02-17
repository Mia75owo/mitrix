#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "idt/idt.h"

void keyboard_init();
void keyboard_handler(CPUState* frame);

// clang-format off
#define KEYCODE_UP     100
#define KEYCODE_LEFT   101
#define KEYCODE_DOWN   102
#define KEYCODE_RIGHT  103
#define KEYCODE_MOD    104
#define KEYCODE_CTRL   105
#define KEYCODE_ESCAPE 106

#define KEYCODE_F1     110
#define KEYCODE_F2     111
#define KEYCODE_F3     112
#define KEYCODE_F4     113
#define KEYCODE_F5     114
#define KEYCODE_F6     115
#define KEYCODE_F7     116
#define KEYCODE_F8     117
#define KEYCODE_F9     118
#define KEYCODE_F10    119
#define KEYCODE_F11    120
#define KEYCODE_F12    121
// clang-format on

typedef struct {
    u8 reserved : 1;

    u8 pressed : 1;
    u8 shift : 1;
    u8 ctrl : 1;
    u8 alt : 1;
    u8 extended : 1;
    u8 special : 1;

    u8 raw;
    char c;
} KeyEvent;

#endif
