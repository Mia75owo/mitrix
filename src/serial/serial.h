#ifndef SERIAL_H_
#define SERIAL_H_

#include <stdarg.h>

void serial_init();

void serial_putchar(char c);
void serial_puts(char* str);

void serial_vprintf(const char* format, va_list va);
void serial_printf(const char* format, ...);

#endif
