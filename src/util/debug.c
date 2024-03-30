#include "debug.h"

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

void kassert(u32 cond, const char* file, u32 line, const char* err) {
    if (cond) return;

    tty_clear();
    tty_set_cursor(0);
    tty_color(2000, 0x40);

    tty_puts((char*)file);
    tty_puts(":");
    tty_put_num(line, 10);
    tty_puts(" assert failed: ");
    tty_puts((char*)err);

    serial_puts((char*)file);
    serial_puts(":");
    serial_put_num(line, 10);
    serial_puts(" assert failed: ");
    serial_puts((char*)err);

    abort();
}

void abort() {
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}
