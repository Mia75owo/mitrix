#ifndef TSS_H_
#define TSS_H_

#include "util/types.h"

typedef struct {
    u16 previous_task, _previous_task_UU;
    u32 esp0;
    u16 ss0, _ss0_UU;
    u32 esp1;
    u16 ss1, _ss1_UU;
    u32 esp2;
    u16 ss2, _ss2_UU;
    u32 cr3;
    u32 eip, eflags, eax, ecx, edx, ebx, esp, ebp, esi, edi;
    u16 es, _es_UU;
    u16 cs, _cs_UU;
    u16 ss, _ss_UU;
    u16 ds, _ds_UU;
    u16 fs, _fs_UU;
    u16 gs, _gs_UU;
    u16 ldt_selector, _ldt_selector_UU;
    u16 debug_flag, io_map;
} __attribute__((packed)) TSS;

#endif
