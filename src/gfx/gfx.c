#include "gfx.h"

#include "disk/mifs.h"
#include "memory/memory.h"
#include "util/debug.h"
#include "util/mem.h"

static u8* font;

bool gfx_ready = false;

static gfx_info gfx;
u32* screen = (u32*)KERNEL_GFX;

void gfx_init(gfx_info info) {
    gfx = info;

    u32 size_bytes = gfx.width * gfx.height * gfx.bpp;
    u32 needed_page_count = CEIL_DIV(size_bytes, 0x1000);

    for (u32 i = 0; i < needed_page_count; i++) {
        u32 offset = i * 0x1000;
        memory_map_page((u32)(KERNEL_GFX + offset), ((u32)gfx.addr) + offset, 0);
    }

    FilePtr font_file = mifs_file("kernelfont.raw");
    assert_msg(font_file.addr != 0, "'kernelfont.raw' NOT FOUND IN RAMDISK!");
    font = font_file.addr;

    gfx_ready = true;
}

void gfx_pixel(u32 x, u32 y, Color color) {
    if (!gfx_ready) return;
    screen[y * gfx.width + x] = color;
}

void gfx_fill(Color color) {
    if (!gfx_ready) return;
    for (u32 i = 0; i < gfx.width * gfx.height; i++) {
        screen[i] = color;
    }
}

void gfx_rect(u32 x, u32 y, u32 width, u32 height, Color color) {
    if (!gfx_ready) return;
    for (u32 i = x; i < x + width; i++) {
        for (u32 j = y; j < y + height; j++) {
            screen[j * gfx.width + i] = color;
        }
    }
}

void gfx_box(u32 x, u32 y, u32 width, u32 height, u32 border, Color color) {
    if (!gfx_ready) return;
    gfx_rect(x, y, width, border, color);
    gfx_rect(x, y + height - border, width, border, color);

    gfx_rect(x, y, border, height, color);
    gfx_rect(x + width - border, y, border, height, color);
}

void gfx_char(u32 x, u32 y, unsigned char c, Color fg, Color bg) {
    if (!gfx_ready) return;
    u32 local_offset_x = (c % 16);
    u32 local_offset_y = (c / 16);

    u32 offset = (local_offset_x * 16) + (local_offset_y * 256 * 16);

    for (u32 i = 0; i < 16; i++) {
        for (u32 j = 0; j < 16; j++) {

            if (font[j * 256 + i + offset] == 0xFF) {
                if ((bg >> 24) != 0x00) {
                    screen[(j + y) * gfx.width + (i + x)] = bg;
                }
            } else {
                screen[(j + y) * gfx.width + (i + x)] = fg;
            }
        }
    }
}

void gfx_debug(GFX_DEBUG_TYPE kind) {
    if (!gfx_ready) return;
    if (kind == GFX_DEBUG_GRADIENT) {
        u8* screen = (u8*)KERNEL_GFX;
        for (u32 i = 0; i < gfx.width; i++) {
            for (u32 j = 0; j < gfx.height; j++) {
                u32 pos = (j * gfx.width + i) * 4;
                screen[pos + 2] = i / 3.2;
                screen[pos + 1] = j / 3.2;
                screen[pos + 0] = 0xFF;
            }
        }
    } else if (kind == GFX_DEBUG_RECTS) {
        gfx_rect(10, 10, 100, 100, 0xFF0000);
        gfx_rect(50, 40, 100, 100, 0x00FF00);
        gfx_rect(20, 60, 100, 100, 0x0000FF);
    } else if (kind == GFX_DEBUG_FONT_SINGLE) {
        for (int i = 0; i < 255; i++) {
            gfx_char(i * 16, (i * 16 / gfx.width) * 16, i, 0xFFFFFFFF, 0xFFFF0000);
        }
    } else if (kind == GFX_DEBUG_FONT_FILL) {
        for (int i = 0; i < 1750; i++) {
            gfx_char(i * 16, (i * 16 / gfx.width) * 16, i, 0xFFFFFFFF, 0xFFFF0000);
        }
    }
}
