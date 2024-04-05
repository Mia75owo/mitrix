#include "fpu.h"

#include "util/types.h"

void fpu_init() {
    u32 cr4;
    asm volatile("mov %%cr4, %0" : "=r"(cr4));
    cr4 |= 0x200;
    asm volatile("mov %0, %%cr4" ::"r"(cr4));

    u16 v = 0x37F;
    asm volatile("fldcw %0" ::"m"(v));
}
