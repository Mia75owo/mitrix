#include "vtty.h"

#include "config.h"
#include "gfx/gfx.h"
#include "gfx/tty.h"
#include "util/mem.h"

static u16 screen[VTTY_WIDTH * VTTY_HEIGHT];
// clang-format off
static u32 color_map[] = {
    0x000000,
    0x0000A8,
    0x00A800,
    0x00A8A8,
    0xA80000,
    0xA800A8,
    0xA85700,
    0xA8A8A8,
    0x575757,
    0x5757FF,
    0x57FF57,
    0x57FFFF,
    0xFF5757,
    0xFF57FF,
    0xFFFF57,
    0xFFFFFF,
};
// clang-format on

void vtty_init() {
    tty_init((char*)screen, VTTY_WIDTH, VTTY_HEIGHT);
    memset(screen, 0, sizeof(screen));
}

void vtty_render() {
    for (u32 y = 0; y < VTTY_HEIGHT; y++) {
        for (u32 x = 0; x < VTTY_WIDTH; x++) {
            u16 chunk = screen[y * VTTY_WIDTH + x];
            char c = chunk;
            char col = (chunk >> 8) & 0xFF;

            char fg = col & 0xF;
            char bg = (col >> 4) & 0xF;

            gfx_char(x * 16, y * 16, c, color_map[(u32)fg],
                     color_map[(u32)bg] | 0xFF000000);
        }
    }
}

void vtty_render_last_line(u32 from) {
    for (u32 x = from; x < VTTY_WIDTH; x++) {
        u16 chunk = screen[(VTTY_HEIGHT - 1) * VTTY_WIDTH + x];
        char c = chunk;
        char col = (chunk >> 8) & 0xFF;

        char fg = col & 0xF;
        char bg = (col >> 4) & 0xF;

        gfx_char(x * 16, (VTTY_HEIGHT - 1) * 16, c, color_map[(u32)fg],
                 color_map[(u32)bg] | 0xFF000000);
    }
}
