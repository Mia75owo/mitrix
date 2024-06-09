#ifndef GFX_H_
#define GFX_H_

#include "types.h"

typedef u32 Color;

typedef struct {
    u32 width;
    u32 height;
    u32 bpp;
    u32 pitch;
    u32* addr;
} GfxInfo;

void gfx_init(GfxInfo info);
void gfx_pixel(u32 x, u32 y, Color color);
void gfx_fill(Color color);
void gfx_rect(u32 x, u32 y, u32 width, u32 height, Color color);
void gfx_box(u32 x, u32 y, u32 width, u32 height, u32 border, Color color);

void gfx_load_font(u32* buffer);
void gfx_char(u32 x, u32 y, unsigned char c, Color fg, Color bg);
void gfx_write(u32 x, u32 y, const char* str, Color fg, Color bg);

#endif
