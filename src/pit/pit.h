#ifndef PIC_H_
#define PIC_H_

#include "util/types.h"
#include "idt/idt.h"

typedef struct {
    u64 tics;
    u32 freq;
    bool enabled;
} PIT;

void pit_init(u32 freq);
void pit_handle_int(InterruptFrame* frame);

#endif
