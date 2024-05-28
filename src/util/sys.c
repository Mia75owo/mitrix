#include "sys.h"

#include "pit/pit.h"
#include "util/debug.h"
#include "util/port.h"

void spin_halt() {
    while (1) halt();
}

void halt() { asm volatile("hlt"); }

void abort() {
    asm volatile("cli");
    spin_halt();
}

void reboot() {
    u8 good = 0x02;
    while (good & 0x02) good = inb(0x64);
    outb(0x64, 0xFE);
    halt();
}

void shutdown() {
    // NOTE: only works on BOCHS/old QEMU
    outw(0xB004, 0x2000);

    // NOTE: only works on QEMU
    outw(0x604, 0x2000);

    // NOTE: only works on VirtualBox
    outw(0x4004, 0x3400);
}

void sleep(u32 ms) {
    asm volatile("sti");
    u64 tics = pit_get_tics();
    while ((tics + ms) > pit_get_tics()) {
    }
}

static u32 cli_depth = 0;
static u32 cli_initially_enabled = false;

void cli_push() {
    if (cli_depth == 0) {
        cli_initially_enabled = eflags_check(EF_INTERRUPTS);
    }
    cli_depth++;
    asm volatile("cli");
}
void cli_pop() {
    assert(cli_depth > 0);
    cli_depth--;

    if (cli_depth == 0 && cli_initially_enabled) {
        asm volatile("sti");
    }
}
