#include "tss.h"

TSS tss;
u32 tss_size = sizeof(tss);

extern void tss_flush();

void tss_update_esp0(u32 esp0) { tss.esp0 = esp0; }
