#include "sys.h"

#include "util/port.h"

void spin_halt() {
    while (1)
        ;
}

void halt() {
    asm volatile("cli");
    while (1) {
        asm volatile("hlt");
    }
}

void abort() { halt(); }

void reboot() {
    u8 good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
    halt();
}
