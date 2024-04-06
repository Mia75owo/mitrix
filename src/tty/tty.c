#include "tty.h"

#include <stdarg.h>

#include "memory/memory.h"
#include "util/mem.h"
#include "util/port.h"
#include "util/types.h"

#define TTY_X 80
#define TTY_Y 25

#define TTY_CTRL 0x3D4
#define TTY_DATA 0x3D5

static char* screen = KMEM(0xB8000);

void tty_set_cursor(u16 pos) {
    outb(TTY_CTRL, 14);
    outb(TTY_DATA, (u8)(pos >> 8));
    outb(TTY_CTRL, 15);
    outb(TTY_DATA, (u8)(pos & 0xFF));
}

u16 tty_get_cursor() {
    u16 pos = 0;

    outb(TTY_CTRL, 14);
    pos += inb(TTY_DATA) << 8;
    outb(TTY_CTRL, 15);
    pos += inb(TTY_DATA);

    return pos;
}
u16 tty_get_cursor_x() {
    u16 pos = tty_get_cursor();
    return pos % TTY_X;
}
u16 tty_get_cursor_y() {
    u16 pos = tty_get_cursor();
    return pos / TTY_X;
}

void tty_scroll(u16 lines) {
    if (lines >= TTY_Y) return;

    memcpy(screen, &screen[TTY_X * lines * 2], TTY_X * (TTY_Y - lines) * 2);
    memset(&screen[(TTY_X * TTY_Y * 2) - (lines * TTY_X * 2)], 0,
           TTY_X * TTY_Y * 2);
}

void tty_clear() { memset(screen, 0, TTY_X * TTY_Y * 2); }

void tty_debug() {
    for (u16 i = 0; i < TTY_X * TTY_Y * 2; i++) {
        screen[i] = i;
    }
}

void tty_putchar(char c) {
    u16 x = tty_get_cursor_x();
    u16 y = tty_get_cursor_y();

    if (c == '\n') {
        x = 0;
        y++;
    } else if (c == '\b') {
        tty_backspace(1);
        return;
    } else {
        x++;
        if (x >= TTY_X) {
            x = 0;
            y++;
        }
    }

    if (y >= TTY_Y) {
        y = TTY_Y - 1;
        tty_scroll(1);
    }

    u16 pos = y * TTY_X + x;

    tty_set_cursor(pos);

    if (c != '\n' && c != '\0' && c != '\b') {
        screen[(pos - 1) * 2] = c;
    }
}

void tty_putcol(char c) {
    u16 pos = tty_get_cursor();
    screen[(pos * 2) + 1] = c;
}
void tty_color(u16 len, char c) {
    u16 to = tty_get_cursor() + len;
    for (u16 i = tty_get_cursor(); (i < to) && (i < TTY_X * TTY_Y); i++) {
        screen[i * 2 + 1] = c;
    }
}

void tty_backspace(u32 times) {
    u16 pos = tty_get_cursor();

    while ((pos > 0) && (times-- > 0)) {
        screen[(pos - 1) * 2] = 0;
        do {
            pos--;
        } while (screen[(pos - 1) * 2] == 0 && pos % TTY_X != 0);

        tty_set_cursor(pos);
    }
}

void tty_puts(char* str) {
    while (*str != '\0') tty_putchar(*str++);
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

                while (*str != '\0') {
                    tty_putcol(color);
                    tty_putchar(*str++);
                }

                format += 2;
                continue;
            } else if (format[1] == 'n') {
                const u64 num = va_arg(va, const u64);
                itoa(buf, num, 32, 10);
                char* str = buf;

                while (*str != '\0') {
                    tty_putcol(color);
                    tty_putchar(*str++);
                }

                format += 2;
                continue;
            } else if (format[1] == 'x') {
                const u64 num = va_arg(va, const u64);
                itoa(buf, num, 32, 16);
                char* str = buf;

                tty_color(2, color);
                tty_puts("0x");

                while (*str != '\0') {
                    tty_putcol(color);
                    tty_putchar(*str++);
                }

                format += 2;
                continue;
            } else if (format[1] == 'c') {
                const char c = va_arg(va, const u32);

                tty_putcol(color);
                tty_putchar(c);

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
                // TODO: this doesn't support printing ']' for now

                char* end_num = strchr(format, '|');
                if (end_num == NULL || end_num == &format[2]) {
                    format += 1;
                    continue;
                }

                *end_num = '\0';
                u32 val = atoi((char*)&format[2], 10);
                *end_num = '|';

                char* end = strchr(format, ']');
                if (end == NULL || end == end_num + 1) {
                    format += 1;
                    continue;
                }

                for (u32 i = 0; i < val; i++) {
                    char* str = end_num + 1;
                    while (str != end) {
                        tty_putcol(color);
                        tty_putchar(*str);

                        str++;
                    }
                }

                format = end + 1;
                continue;
            }
        }

        tty_putcol(color);
        tty_putchar(*format);
        format++;
    }
}

void tty_getstr(char* dest, u32 src, u32 len) {
    while (len != 0 && src < TTY_X * TTY_Y) {
        *dest++ = screen[src++ * 2];
    }
}

void tty_reset() {
    tty_set_cursor(0);
    tty_clear();
    tty_color(TTY_X * TTY_Y, 0x03);
}
