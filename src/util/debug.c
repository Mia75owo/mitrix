#include "debug.h"
#include <stdarg.h>

#include "util/mem.h"
#include "util/serial.h"
#include "tty/tty.h"

void klog(char* str) {
    u32 len = strlen(str);
    tty_color(len, 0x40);
    tty_puts(str);

    serial_puts(str);
}
void klog_num(u64 num, u16 base) {
    static char buf[32];
    itoa(buf, num, 32, base);
    klog(buf);
}

void kpanic_(const char* first, ...) {
    tty_clear();
    tty_set_cursor(0);
    tty_color(2000, 0x40);

    va_list va;
    va_start(va, first);

    for (const char* arg = first; arg != (char*)0; arg = va_arg(va, const char*)) {
        tty_puts((char*)arg);
        serial_puts((char*)arg);
    }

    va_end(va);

    abort();
}

void abort() {
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}
