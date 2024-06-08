#ifndef MOUSE_H_
#define MOUSE_H_

#include "idt/idt.h"
#include "util/types.h"

typedef struct {
    i32 x;
    i32 y;
    u8 button_left : 1;
    u8 button_right : 1;
} Mouse;

typedef struct {
    u8 reserved : 1;

    u8 button_left : 1;
    u8 button_right : 1;
    i8 delta_x;
    i8 delta_y;
} MouseEvent;

void mouse_init();
void mouse_handler(CPUState* frame);

#endif
