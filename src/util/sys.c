#include "sys.h"

#include "pit/pit.h"
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

void sleep(u32 ms) {
    asm volatile("sti");
    u64 tics = pit_get_tics();
    while ((tics + ms) > pit_get_tics()) {
    }
}
