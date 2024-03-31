#include "debug.h"
#include <stdarg.h>

#include "util/serial.h"
#include "tty/tty.h"

void klog(char* format, ...) {
    va_list va;
    va_start(va, format);

    tty_printf(format, va);
    // TODO: serial_printf
}

void kpanic(const char* format, ...) {
    tty_clear();
    tty_set_cursor(0);
    tty_color(2000, 0x40);

    va_list va;
    va_start(va, format);

    tty_printf(format, va);
    // TODO: serial_printf

    va_end(va);

    abort();
}

void abort() {
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}
