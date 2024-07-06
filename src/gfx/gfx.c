#include "gfx.h"

// #include "config.h"

#include "disk/mifs.h"
#include "memory/kmalloc.h"
#include "memory/memory.h"
#include "util/debug.h"
#include "util/mem.h"

static u8* font;
static u8* logo;

static bool gfx_ready = false;
static bool double_buffering = true;

static gfx_info gfx;
static volatile u32* const frontbuffer = (u32*)KERNEL_GFX;
static u32* restrict backbuffer;

static u32* restrict screen;

#ifdef DEBUG
#define CHECK_INITIALIZED() \
    if (!gfx_ready) return;
#else
#define CHECK_INITIALIZED()
#endif

void gfx_init(gfx_info info) {
    gfx = info;

    u32 size_bytes = SCREEN_X * SCREEN_Y * gfx.bpp;
    u32 needed_page_count = CEIL_DIV(size_bytes, 0x1000);

    for (u32 i = 0; i < needed_page_count; i++) {
        u32 offset = i * 0x1000;
        memory_map_page((u32)(KERNEL_GFX + offset), ((u32)gfx.addr) + offset,
                        0);
    }

    FilePtr font_file = mifs_file("kernelfont.raw");
    assert_msg(font_file.addr != 0, "'kernelfont.raw' NOT FOUND IN RAMDISK!");
    font = font_file.addr;

    FilePtr logo_file = mifs_file("mitrix_logo.raw");
    assert_msg(logo_file.addr != 0, "'mitrix_logo.raw' NOT FOUND IN RAMDISK!");
    logo = logo_file.addr;

    backbuffer = (u32*)kmalloc(size_bytes);
    screen = backbuffer;

    gfx_ready = true;
}

void gfx_pixel(u32 x, u32 y, Color color) {
    CHECK_INITIALIZED();
    screen[y * SCREEN_X + x] = color;
}

void gfx_fill(Color color) {
    CHECK_INITIALIZED();
    for (u32 i = 0; i < SCREEN_X * SCREEN_Y; i++) {
        screen[i] = color;
    }
}

void gfx_rect(u32 x, u32 y, u32 width, u32 height, Color color) {
    CHECK_INITIALIZED();
    for (u32 i = x; i < x + width; i++) {
        for (u32 j = y; j < y + height; j++) {
            screen[j * SCREEN_X + i] = color;
        }
    }
}

void gfx_box(u32 x, u32 y, u32 width, u32 height, u32 border, Color color) {
    CHECK_INITIALIZED();
    gfx_rect(x, y, width, border, color);
    gfx_rect(x, y + height - border, width, border, color);

    gfx_rect(x, y, border, height, color);
    gfx_rect(x + width - border, y, border, height, color);
}

void gfx_char(u32 x, u32 y, unsigned char c, Color fg, Color bg) {
    CHECK_INITIALIZED();
    u32 local_offset_x = (c % 16);
    u32 local_offset_y = (c / 16);

    u32 offset = (local_offset_x * 16) + (local_offset_y * 256 * 16);

    for (u32 i = 0; i < 16; i++) {
        for (u32 j = 0; j < 16; j++) {
            if (font[j * 256 + i + offset] == 0xFF) {
                if ((bg >> 24) != 0x00) {
                    screen[(j + y) * SCREEN_X + (i + x)] = bg;
                }
            } else {
                screen[(j + y) * SCREEN_X + (i + x)] = fg;
            }
        }
    }
}

void gfx_clone(u32 x, u32 y, u32 width, u32 height, u32* restrict source) {
    for (u32 j = 0; j < height; j++) {
        for (u32 i = 0; i < width; i++) {
            screen[(j + y) * SCREEN_X + (i + x)] = source[j * width + i];
        }
    }
}

void gfx_display_backbuffer() {
    if (!double_buffering) return;

    u64* const from = (u64* const)backbuffer;
    u64* restrict const to = (u64* restrict const)frontbuffer;

    for (u32 i = 0; i < (SCREEN_X * SCREEN_Y / 2); i++) {
        to[i] = from[i];
    }
}

void gfx_doublebuffering(bool enable) {
    if (enable) {
        screen = backbuffer;
    } else {
        screen = (u32* restrict)frontbuffer;
    }
    double_buffering = enable;
}

bool gfx_get_doublebuffering() {
    return double_buffering;
}

void gfx_logo() {
    CHECK_INITIALIZED();

    u32 offset = (SCREEN_X - 800) / 2;
    for (u32 i = 0; i < 800; i++) {
        for (u32 j = 0; j < 208; j++) {
            if (logo[j * 800 + i] == 0xFF) {
                // screen[j * SCREEN_X + i + offset] = 0xFF000000;
            } else {
                screen[j * SCREEN_X + i + offset] = 0xFFFFFFFF;
            }
        }
    }
}

void gfx_debug(GFX_DEBUG_TYPE kind) {
    CHECK_INITIALIZED();
    if (kind == GFX_DEBUG_GRADIENT) {
        u8* screen = (u8*)KERNEL_GFX;
        for (u32 i = 0; i < SCREEN_X; i++) {
            for (u32 j = 0; j < SCREEN_Y; j++) {
                u32 pos = (j * SCREEN_X + i) * 4;
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
            gfx_char(i * 16, (i * 16 / SCREEN_X) * 16, i, 0xFFFFFFFF,
                     0xFFFF0000);
        }
    } else if (kind == GFX_DEBUG_FONT_FILL) {
        for (int i = 0; i < 1750; i++) {
            gfx_char(i * 16, (i * 16 / SCREEN_X) * 16, i, 0xFFFFFFFF,
                     0xFFFF0000);
        }
    }
}
