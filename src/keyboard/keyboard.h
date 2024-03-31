#ifndef KEYBOARD_H_
#define KEYBOARD_H_

#include "idt/idt.h"

void keyboard_init();
void keyboard_handler(InterruptFrame* frame);

#endif
