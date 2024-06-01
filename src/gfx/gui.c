#include "gui.h"

#include "gfx/gfx.h"
#include "gfx/tty.h"
#include "gfx/vtty.h"
#include "memory/kmalloc.h"
#include "memory/memory.h"
#include "pit/pit.h"
#include "shell/shell.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/sys.h"

static GUI gui;
static void gui_check_command();
static void gui_add_history(char* str);
static void gui_user_input_key_event(KeyEvent evt);

void gui_init_early_tty() { vtty_init(); }

void gui_init(u32 width, u32 height) {
    gui.width = width;
    gui.height = height;
    gui.prompt_cursor = 0;
    gui.history_index = 0;
    gui.getting_user_input = false;
    gui.user_input_ready = false;
    gui.entire_redraw = true;
    memset(gui.prompt_buffer, 0, GUI_PROMPT_SIZE);
    memset(gui.prompt_history, 0, sizeof(gui.prompt_history));

    shell_init();
}
void gui_redraw() {
    if (gui.entire_redraw) {
        gfx_fill(0xFF000000);
        vtty_render();
        gui.entire_redraw = false;
    } else if (tty_should_redraw()) {
        vtty_render();
    }
    vtty_render_last_line(tty_redraw_last_from());

    u32 cursor_pos = 0;
    if (gui.getting_user_input) {
        gfx_box(0, gui.height - 40, gui.width, 40, 4, 0xFF57FF57);

        u32 chars_to_draw = (gui.width - 8 - 20) / 16;
        for (u32 i = 0; i < chars_to_draw; i++) {
            gfx_char(10 + i * 16, gui.height - 28, gui.user_input[i],
                     0xFFFFFFFF, 0xFF000000);
        }
        cursor_pos = gui.user_input_cursor;
    } else {
        gfx_box(0, gui.height - 40, gui.width, 40, 4, 0xFFFFFFFF);

        u32 chars_to_draw = (gui.width - 8 - 20) / 16;
        for (u32 i = 0; i < chars_to_draw; i++) {
            gfx_char(10 + i * 16, gui.height - 28, gui.prompt_buffer[i],
                     0xFFFFFFFF, 0xFF000000);
        }
        cursor_pos = gui.prompt_cursor;
    }

    if (pit_get_tics() % 1000 > 500) {
        gfx_rect(10 + cursor_pos * 16, gui.height - 28, 2, 16, 0xFFFFFFFF);
    } else {
        gfx_rect(10 + cursor_pos * 16, gui.height - 28, 2, 16, 0xFF57FF57);
    }
}
void gui_trigger_entire_redraw() { gui.entire_redraw = true; }

void gui_key_event(KeyEvent evt) {
    // Ignore key release
    if (!evt.pressed) {
        return;
    }

    if (gui.getting_user_input) {
        gui_user_input_key_event(evt);
        return;
    }

    if (evt.alt) {
        switch (evt.c) {
            case 'd':
                gfx_debug(GFX_DEBUG_FONT_FILL);
                break;
            case 'e':
                asm volatile("int $1");
                break;
            case 'r':
                reboot();
                break;
            case 'm':
                memory_print_info();
                break;
            case 'k':
                kmalloc_print_info();
                break;
        }
        return;
    }

    // Cycle through history
    if (evt.special) {
        if (evt.c == KEYCODE_UP) {
            // history_index 0 is the current buffer
            gui.history_index++;
            if (gui.history_index >= GUI_PROMPT_HISTORY_SIZE) {
                gui.history_index = GUI_PROMPT_HISTORY_SIZE;
            }

            // reached top of used history
            if (gui.prompt_history[gui.history_index - 1][0] == 0) {
                gui.history_index--;
                return;
            }

            memcpy(gui.prompt_buffer, gui.prompt_history[gui.history_index - 1],
                   GUI_PROMPT_SIZE);
        } else if (evt.c == KEYCODE_DOWN) {
            if (gui.history_index <= 0) {
                return;
            }
            gui.history_index--;

            // reached normal buffer
            if (gui.history_index == 0) {
                memset(gui.prompt_buffer, 0, GUI_PROMPT_SIZE);
            } else {
                memcpy(gui.prompt_buffer,
                       gui.prompt_history[gui.history_index - 1],
                       GUI_PROMPT_SIZE);
            }
        }

        gui.prompt_cursor = strlen(gui.prompt_buffer);
        return;
    }

    // Update prompt
    char c = evt.c;

    // Backspace
    if (c == '\b') {
        if (gui.prompt_cursor > 0) gui.prompt_cursor--;
        gui.prompt_buffer[gui.prompt_cursor] = 0;
        return;
    }

    // Enter
    if (c == '\n') {
        klog("%07%s\n", gui.prompt_buffer);

        if (strlen(gui.prompt_buffer) == 0) {
            return;
        }

        if (strncmp(gui.prompt_buffer, gui.prompt_history[0],
                    GUI_PROMPT_SIZE) != 0) {
            gui_add_history(gui.prompt_buffer);
        }
        gui.history_index = 0;

        gui_check_command();

        // reset prompt
        memset(gui.prompt_buffer, 0, GUI_PROMPT_SIZE);
        gui.prompt_cursor = 0;
        return;
    }

    gui.prompt_buffer[gui.prompt_cursor] = c;

    gui.prompt_cursor++;
    if (gui.prompt_cursor >= GUI_PROMPT_SIZE) {
        gui.prompt_cursor = GUI_PROMPT_SIZE - 1;
    }
}

static void gui_user_input_key_event(KeyEvent evt) {
    // Update prompt
    char c = evt.c;

    if (gui.user_input_ready) {
        return;
    }

    // Backspace
    if (c == '\b') {
        if (gui.user_input > 0) gui.user_input_cursor--;
        gui.user_input[gui.user_input_cursor] = 0;
        return;
    }

    gui.user_input[gui.user_input_cursor] = c;

    gui.user_input_cursor++;
    if (gui.user_input_cursor >= GUI_PROMPT_SIZE) {
        gui.user_input_cursor = GUI_PROMPT_SIZE - 1;
    }

    if (gui.user_input_cursor >= gui.user_input_wanted_len) {
        gui.user_input_ready = true;
    }

    klog("%c", c);
}

char* gui_get_user_input(u32 len) {
    assert(len <= GUI_PROMPT_SIZE);

    memset(gui.user_input, 0, GUI_PROMPT_SIZE);
    gui.user_input_cursor = 0;
    gui.getting_user_input = true;
    gui.user_input_ready = false;
    gui.user_input_wanted_len = len;

    while (!gui.user_input_ready) {
    }

    gui.getting_user_input = false;

    return gui.user_input;
}

static void gui_check_command() { shell_execute_command(gui.prompt_buffer); }
static void gui_add_history(char* str) {
    // shift array
    for (u32 i = GUI_PROMPT_HISTORY_SIZE - 1; i > 0; i--) {
        memcpy(gui.prompt_history[i], gui.prompt_history[i - 1],
               GUI_PROMPT_SIZE);
    }

    memcpy(gui.prompt_history[0], str, GUI_PROMPT_SIZE);
}
