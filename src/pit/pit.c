#include "pit.h"

#include "tty/tty.h"
#include "util/port.h"
#include "util/mem.h"

static PIT pit = {0, 0, 1};

void pit_init(u32 freq) {
    pit.freq = freq;

    DWord divisor;
    divisor.val = 1193180 / freq;

    outb(0x43, 0x36);
    outb(0x40, divisor.lower.lower);
    outb(0x40, divisor.lower.higher);

    set_isr_function(32, pit_handle_int);
}

void pit_handle_int(InterruptFrame* frame) {
    (void)frame;

    pit.tics++;

    u16 cursor = tty_get_cursor();
    tty_set_cursor(2000 - 11);
    tty_color(11, 0xa0);
    tty_printf("%A0T: %n", (u64)pit.tics);
    tty_set_cursor(cursor);
}
