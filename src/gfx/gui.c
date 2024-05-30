#include "gui.h"

#include "disk/mifs.h"
#include "gfx/gfx.h"
#include "gfx/tty.h"
#include "gfx/vtty.h"
#include "memory/memory.h"
#include "pit/pit.h"
#include "shell/shell.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/sys.h"

static GUI gui;

static void gui_check_command();

void gui_init_early_tty() { vtty_init(); }

void gui_init(u32 width, u32 height) {
    gui.width = width;
    gui.height = height;
    gui.prompt_cursor = 0;
    memset(gui.prompt_buffer, 0, GUI_PROMPT_SIZE);

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

    if (evt.special) {
        return;
    }

    // Update prompt
    char c = evt.c;
    if (c == '\b') {
        if (gui.prompt_cursor > 0) gui.prompt_cursor--;
        gui.prompt_buffer[gui.prompt_cursor] = 0;
        return;
    } else if (c == '\n') {
        klog("%s\n", gui.prompt_buffer);

        gui_check_command();

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
