#ifndef GUI_H_
#define GUI_H_

#include "keyboard/keyboard.h"
#include "util/types.h"

#define GUI_PROMPT_SIZE 256

typedef struct {
    u32 width;
    u32 height;

    char prompt_buffer[GUI_PROMPT_SIZE];
    u32 prompt_cursor;
} GUI;

void gui_init_early_tty();
void gui_init(u32 width, u32 height);
void gui_redraw();

void gui_key_event(KeyEvent evt);

#endif
