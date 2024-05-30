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

void gui_init_early_tty() { vtty_init(); }

void gui_init(u32 width, u32 height) {
    gui.width = width;
    gui.height = height;
    gui.prompt_cursor = 0;
    gui.history_index = 0;
    memset(gui.prompt_buffer, 0, GUI_PROMPT_SIZE);
    memset(gui.prompt_history, 0, sizeof(gui.prompt_history));

    shell_init();
}
void gui_redraw() {
    if (tty_should_redraw()) {
        vtty_render();
    }
    vtty_render_last_line(tty_redraw_last_from());

    gfx_box(0, gui.height - 40, gui.width, 40, 4, 0xFFFFFFFF);

    u32 chars_to_draw = (gui.width - 8 - 20) / 16;
    for (u32 i = 0; i < chars_to_draw; i++) {
        gfx_char(10 + i * 16, gui.height - 28, gui.prompt_buffer[i], 0xFFFFFFFF,
                 0xFF000000);
    }

    if (pit_get_tics() % 1000 > 500) {
        gfx_rect(10 + gui.prompt_cursor * 16, gui.height - 28, 2, 16, 0xFFFFFFFF);
    } else {
        gfx_rect(10 + gui.prompt_cursor * 16, gui.height - 28, 2, 16, 0xFF57FF57);
    }
}

void gui_key_event(KeyEvent evt) {
    // Ignore key release
    if (!evt.pressed) {
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

            memcpy(gui.prompt_buffer, gui.prompt_history[gui.history_index - 1], GUI_PROMPT_SIZE);
        } else if (evt.c == KEYCODE_DOWN) {
            if (gui.history_index <= 0) {
                return;
            }
            gui.history_index--;

            // reached normal buffer
            if (gui.history_index == 0) {
                memset(gui.prompt_buffer, 0, GUI_PROMPT_SIZE);
            } else {
                memcpy(gui.prompt_buffer, gui.prompt_history[gui.history_index - 1], GUI_PROMPT_SIZE);
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

        if (strncmp(gui.prompt_buffer, gui.prompt_history[0], GUI_PROMPT_SIZE) != 0) {
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

static void gui_check_command() { shell_execute_command(gui.prompt_buffer); }
static void gui_add_history(char* str) {
    // shift array
    for (u32 i = GUI_PROMPT_HISTORY_SIZE - 1; i > 0; i--) {
        memcpy(gui.prompt_history[i], gui.prompt_history[i - 1], GUI_PROMPT_SIZE);
    }

    // memcpy(gui.prompt_history[1], gui.prompt_history[0], GUI_PROMPT_SIZE * (GUI_PROMPT_HISTORY_SIZE - 1));
    memcpy(gui.prompt_history[0], str, GUI_PROMPT_SIZE);
}
