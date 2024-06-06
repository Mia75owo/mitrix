#include "tty.h"

#include <stdarg.h>

#include "gfx/vtty.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/types.h"

static u32 TTY_X;
static u32 TTY_Y;
static u32 TTY_WRITE_START;

static bool should_redraw = false;
static u32 redraw_last_from = 0;

char* tty_buffer = 0;
static u32 cursor = 0;

void tty_init(char* screen_addr, u32 width, u32 height) {
    TTY_X = width;
    TTY_Y = height;
    TTY_WRITE_START = width * (height - 1);
    tty_buffer = screen_addr;
}

void tty_scroll(u32 lines) {
    if (lines >= TTY_Y) return;

    memcpy(tty_buffer, &tty_buffer[TTY_X * lines * 2],
           TTY_X * (TTY_Y - lines) * 2);
    memset(&tty_buffer[(TTY_X * TTY_Y * 2) - (lines * TTY_X * 2)], 0,
           (lines * TTY_X * 2));

    should_redraw = true;
}

void tty_clear() {
    memset(tty_buffer, 0, TTY_X * TTY_Y * 2);
    cursor = 0;
    should_redraw = true;
}

void tty_putchar(char c, u8 color) {
    if (c == '\n') {
        tty_scroll(1);
        cursor = 0;
        return;
    }

    tty_buffer[(TTY_WRITE_START + cursor) * 2] = c;
    tty_buffer[(TTY_WRITE_START + cursor) * 2 + 1] = color;

    cursor++;
    if (cursor >= TTY_X) {
        tty_scroll(1);
        cursor = 0;
    }

    redraw_last_from = cursor;
}

void tty_puts(char* str, u8 color) {
    while (*str != '\0') tty_putchar(*str++, color);
}

void tty_putbuf(char* str, u32 len, u8 color) {
    for (u32 i = 0; i < len; i++) {
        tty_putchar(str[i], color);
    }
}

void tty_printf(const char* format, ...) {
    va_list va;
    va_start(va, format);

    tty_vprintf(format, va);

    va_end(va);
}

void tty_vprintf(const char* format, va_list va) {
    u8 color = 0x03;

    static char buf[32];

    while (*format != '\0') {
        if (*format == '%' && format[1] != '%') {
            if (format[1] == 's') {
                const char* str = va_arg(va, const char*);

                tty_puts((char*)str, color);

                format += 2;
                continue;
            } else if (format[1] == 'n') {
                const u64 num = va_arg(va, const u64);
                itoa(buf, num, 32, 10);
                char* str = buf;

                tty_puts(str, color);

                format += 2;
                continue;
            } else if (format[1] == 'x') {
                const u64 num = va_arg(va, const u64);
                itoa(buf, num, 32, 16);
                char* str = buf;

                tty_puts("0x", color);
                tty_puts(str, color);

                format += 2;
                continue;
            } else if (format[1] == 'c') {
                const char c = va_arg(va, const u32);

                tty_putchar(c, color);

                format += 2;
                continue;
            } else if (cishex(format[1]) &&
                       !(format[1] >= 'a' && format[1] <= 'f')) {
                color = (ctoi(format[1]) & 0xf) << 4;
                color |= (ctoi(format[2]) & 0xf) << 0;

                format += 3;
                continue;
            } else if (format[1] == '[') {
                // %[123|a]
                // %[@|a] (the whole line)
                // TODO: this doesn't support printing ']' for now

                char* end_num = strchr(format, '|');
                if (end_num == NULL || end_num == &format[2]) {
                    format += 1;
                    continue;
                }

                u32 val;
                if (format[2] == '@') {
                    val = VTTY_WIDTH;
                } else {
                    *end_num = '\0';
                    val = atoi((char*)&format[2], 10);
                    *end_num = '|';
                }


                char* end = strchr(format, ']');
                if (end == NULL || end == end_num + 1) {
                    format += 1;
                    continue;
                }

                for (u32 i = 0; i < val; i++) {
                    char* str = end_num + 1;
                    while (str != end) {
                        tty_putchar(*str, color);

                        str++;
                    }
                }

                format = end + 1;
                continue;
            }
        }

        tty_putchar(*format, color);
        format++;
    }
}

void tty_reset() {
    cursor = 0;
    tty_clear();
}

void tty_set_color(u8 color) {
    for (u32 i = 0; i < TTY_X * TTY_Y; i++) {
        tty_buffer[i * 2 + 1] = color;
    }
    should_redraw = true;
}

bool tty_should_redraw() { return should_redraw; }
u32 tty_redraw_last_from() { return redraw_last_from; }
