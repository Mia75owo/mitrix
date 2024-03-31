#include "debug.h"
#include <stdarg.h>

#include "serial/serial.h"
#include "tty/tty.h"

void klog(char* format, ...) {
    va_list va;
    va_start(va, format);

    tty_vprintf(format, va);
    serial_vprintf(format, va);

    va_end(va);
}

void kpanic(const char* format, ...) {
    tty_clear();
    tty_set_cursor(0);
    tty_color(2000, 0x40);

    va_list va;
    va_start(va, format);

    tty_vprintf(format, va);
    serial_vprintf(format, va);

    va_end(va);

    abort();
}

void abort() {
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}
