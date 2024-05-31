#ifndef TTY_H_
#define TTY_H_

#include <stdarg.h>
#include "util/types.h"

void tty_init(char* screen_addr, u32 width, u32 height);
void tty_scroll(u32 lines);
void tty_clear();
void tty_putchar(char c, u8 color);
void tty_puts(char* str, u8 color);
void tty_putbuf(char* str, u32 len, u8 color);
void tty_printf(const char* format, ...);
void tty_vprintf(const char* format, va_list va);
void tty_reset();
void tty_set_color(u8 color);
bool tty_should_redraw();
u32 tty_redraw_last_from();

#endif
