#ifndef INTERRUPTS_H_
#define INTERRUPTS_H_

#include "util/types.h"

typedef struct {
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 interrupt, error;

    u32 eip, cs, eflags, usermode_esp, usermode_ss;
} InterruptFrame;

__attribute__((interrupt)) void page_fault_handler(InterruptFrame* frame);

#endif
