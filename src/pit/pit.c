#include "pit.h"

#include "tasks/task_manager.h"
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

void pit_handle_int(CPUState* frame) {
    (void)frame;
    pit.tics++;
    task_manager_schedule();
}

u64 pit_get_tics() {
    return pit.tics;
}
