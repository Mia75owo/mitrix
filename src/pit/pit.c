#include "pit.h"

#include "tty/tty.h"
#include "util/port.h"

static PIT pit = {0, 0, 1};

void pit_init(u32 freq) {
    pit.freq = freq;

    u32 divisor = 1193180 / freq;

    outb(0x43, 0x36);
    outb(0x40, (u8)(divisor >> 0 & 0xFF));
    outb(0x40, (u8)(divisor >> 8 & 0xFF));

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
