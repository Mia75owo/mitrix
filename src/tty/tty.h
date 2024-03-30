#ifndef TTY_H_
#define TTY_H_

#include "util/types.h"

void tty_set_cursor(u16 pos);
u16 tty_get_cursor();
u16 tty_get_cursor_x();
u16 tty_get_cursor_y();

void tty_scroll(u16 lines);

void tty_clear();
void tty_debug();

void tty_putchar(char c);
void tty_putcol(char c);

void tty_color(u16 len, char c);

void tty_puts(char* str);
void tty_put_num(u64 num, u16 base);

#endif