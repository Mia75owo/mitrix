#include "gfx.h"

static GfxInfo gfx;
static u32* screen;

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
