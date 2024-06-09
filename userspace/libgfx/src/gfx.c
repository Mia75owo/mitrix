#include "gfx.h"

static GfxInfo gfx;
static u32* screen;
static u8* font;

void gfx_init(GfxInfo info) {
    gfx = info;
    screen = info.addr;
}

void gfx_pixel(u32 x, u32 y, Color color) { screen[y * gfx.width + x] = color; }

void gfx_fill(Color color) {
    for (u32 i = 0; i < gfx.width * gfx.height; i++) {
        screen[i] = color;
    }
}

void gfx_rect(u32 x, u32 y, u32 width, u32 height, Color color) {
    for (u32 i = x; i < x + width; i++) {
        for (u32 j = y; j < y + height; j++) {
            screen[j * gfx.width + i] = color;
        }
    }
}

void gfx_box(u32 x, u32 y, u32 width, u32 height, u32 border, Color color) {
    gfx_rect(x, y, width, border, color);
    gfx_rect(x, y + height - border, width, border, color);

    gfx_rect(x, y, border, height, color);
    gfx_rect(x + width - border, y, border, height, color);
}

void gfx_load_font(u32* buffer) {
    font = buffer;
}

void gfx_char(u32 x, u32 y, unsigned char c, Color fg, Color bg) {
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
