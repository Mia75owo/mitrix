#include <stdarg.h>
#include "serial/serial.h"
#include "util/types.h"
#include "util/port.h"
#include "util/mem.h"

#define PORT 0x3F8 // COM1

static u8 serial_initialized = 0;

void serial_init() {
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x80);
    outb(PORT + 0, 0x03);
    outb(PORT + 1, 0x00);
    outb(PORT + 3, 0x03);
    outb(PORT + 2, 0xC7);
    outb(PORT + 4, 0x0B);

    serial_initialized = 1;
}

static int port_is_transmit_empty() {
   return inb(PORT + 5) & 0x20;
}

static void port_write(char a) {
   while (port_is_transmit_empty() == 0);
   outb(PORT,a);
}

void serial_putchar(char c) {
    if (!serial_initialized)
        return;
    
    if (c == '\b') {
        port_write('\b');
        port_write(' ');
        port_write('\b');
    } else {
        port_write(c);
    }
}

void serial_puts(char* str) {
    while (*str != '\0') serial_putchar(*str++);
}

void serial_printf(const char* format, ...) {
    va_list va;
    va_start(va, format);

    serial_vprintf(format, va);

    va_end(va);
}

void serial_vprintf(const char* format, va_list va) {
    static char buf[32];

    while (*format != '\0') {
        if (*format == '%' && format[1] != '%') {
            if (format[1] == 's') {
                const char* str = va_arg(va, const char*);

                serial_puts((char*)str);

                format += 2;
                continue;
            } else if (format[1] == 'n') {
                const u64 num = va_arg(va, const u64);
                itoa(buf, num, 32, 10);
                char* str = buf;

                serial_puts((char*)str);

                format += 2;
                continue;
            } else if (format[1] == 'x') {
                const u64 num = va_arg(va, const u64);
                itoa(buf, num, 32, 16);
                char* str = buf;

                serial_puts("0x");
                serial_puts((char*)str);

                format += 2;
                continue;
            } else if (format[1] == 'c') {
                const char c = va_arg(va, const u32);

                serial_putchar(c);

                format += 2;
                continue;
            } else if (cishex(format[1]) && !(format[1] >= 'a' && format[1] <= 'f')) {
                // Ignore color format option
                format += 3;
                continue;
            }
        }

        serial_putchar(*format);
        format++;
    }
}
