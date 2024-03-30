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

void serial_write(char c) {
    if (!serial_initialized)
        return;
    
    port_write(c);
}

void serial_puts(char* str) {
    while (*str != '\0') serial_write(*str++);
}
void serial_put_num(u64 num, u16 base) {
    static char buf[32];
    itoa(buf, num, 32, base);
    serial_puts(buf);
}
