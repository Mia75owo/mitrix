#include "gui.h"

#include "gfx/gfx.h"
#include "gfx/tty.h"
#include "gfx/vtty.h"
#include "memory/memory.h"
#include "pit/pit.h"
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

void gui_key_input(char c) {
    if (c == '\b') {
        if (gui.prompt_cursor > 0) gui.prompt_cursor--;
        gui.prompt_buffer[gui.prompt_cursor] = 0;
        return;
    } else if (c == '\n') {
        klog("\n%s", gui.prompt_buffer);

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

static void gui_check_command() {
    if (strcmp(gui.prompt_buffer, "clear") == 0) {
        tty_clear();
    } else if (strcmp(gui.prompt_buffer, "reboot") == 0) {
        reboot();
    } else if (strcmp(gui.prompt_buffer, "meminfo") == 0) {
        memory_print_info();
    } else if (strcmp(gui.prompt_buffer, "tics") == 0) {
        klog("\n%n", (u64)pit_get_tics());
    } else if (strcmp(gui.prompt_buffer, "debuggfx") == 0) {
        gfx_debug(GFX_DEBUG_FONT_FILL);
        sleep(1000);
        gfx_fill(0xFF000000);
    } else if (strcmp(gui.prompt_buffer, "debugint") == 0) {
        asm volatile("int $2");
    }

}
