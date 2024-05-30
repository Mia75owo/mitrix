#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "idt/idt.h"

void keyboard_init();
void keyboard_handler(CPUState* frame);

#define KEYCODE_UP 100
#define KEYCODE_LEFT 101
#define KEYCODE_DOWN 102
#define KEYCODE_RIGHT 103

typedef struct {
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
