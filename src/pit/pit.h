#ifndef PIC_H_
#define PIC_H_

#include "idt/idt.h"
#include "util/types.h"

typedef struct {
    volatile u64 tics;
    u32 freq;
    bool enabled;
} PIT;

void pit_init(u32 freq);
void pit_handle_int(CPUState* frame);
u64 pit_get_tics();

#endif
