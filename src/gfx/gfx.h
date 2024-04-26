#ifndef GFX_H_
#define GFX_H_

#include "util/types.h"

extern bool gfx_ready;

typedef u32 Color;

typedef struct {
    u32 width;
    u32 height;
    u32 bpp;
    u32 pitch;
    u32* addr;
} gfx_info;

void gfx_init(gfx_info info);

void gfx_fill(Color color);
void gfx_pixel(u32 x, u32 y, Color color);
void gfx_rect(u32 x, u32 y, u32 width, u32 height, Color color);
void gfx_box(u32 x, u32 y, u32 width, u32 height, u32 border, Color color);
void gfx_char(u32 x, u32 y, unsigned char c, Color fg, Color bg);
void gfx_logo();

typedef enum {
    GFX_DEBUG_GRADIENT,
    GFX_DEBUG_RECTS,
    GFX_DEBUG_FONT_SINGLE,
    GFX_DEBUG_FONT_FILL,
} GFX_DEBUG_TYPE;
void gfx_debug(GFX_DEBUG_TYPE kind);

#endif
