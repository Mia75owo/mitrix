#ifndef GUI_H_
#define GUI_H_

#include "keyboard/keyboard.h"
#include "mouse/mouse.h"
#include "util/types.h"

#define GUI_PROMPT_SIZE 256
#define GUI_PROMPT_HISTORY_SIZE 16

typedef struct {
    u32 width;
    u32 height;

    char prompt_buffer[GUI_PROMPT_SIZE];
    char prompt_history[GUI_PROMPT_HISTORY_SIZE][GUI_PROMPT_SIZE];
    u32 history_index;

    u32 prompt_cursor;

    bool getting_user_input;
    bool user_input_ready;
    u32 user_input_wanted_len;
    u32 user_input_cursor;
    char user_input[GUI_PROMPT_SIZE];

    bool entire_redraw;

    Mouse mouse;
} GUI;

void gui_init_early_tty();
void gui_init(u32 width, u32 height);
void gui_redraw();
void gui_trigger_entire_redraw();

void gui_key_event(KeyEvent evt);
void gui_mouse_event(MouseEvent evt);

char* gui_get_user_input(u32 len);

#endif
