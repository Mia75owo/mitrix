#include "tty.h"
#include "util/types.h"
#include "util/port.h"
#include "util/mem.h"

#define TTY_X 80
#define TTY_Y 25

#define TTY_CTRL 0x3D4
#define TTY_DATA 0x3D5

static char* screen = (char*)0xb8000;

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
    if (lines >= TTY_Y)
        return;

    memcpy(screen, &screen[TTY_X * lines * 2], TTY_X * (TTY_Y - lines) * 2);
    memset(&screen[(TTY_X * TTY_Y * 2) - (lines * TTY_X * 2)], 0, TTY_X * TTY_Y * 2);
}

void tty_clear() {
    memset(screen, 0, TTY_X * TTY_Y * 2);
}

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
    screen[(pos - 1) * 2] = c;
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

void tty_puts(char* str) {
    while (*str != '\0') tty_putchar(*str++);
}
void tty_put_num(u64 num, u16 base) {
    static char buf[32];
    itoa(buf, num, 32, base);
    tty_puts(buf);
}
