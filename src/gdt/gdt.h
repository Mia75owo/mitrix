#ifndef GDT_H_
#define GDT_H_

#include "util/types.h"

#define GDT_ENTRIES_COUNT 6

typedef struct {
    u16 limit_low;
    u16 base_low;
    u8 base_mid;
    u8 access;
    u8 limit : 4;
    u8 flags : 4;
    u8 base_high;
} __attribute__((packed)) GDTEntry;

typedef struct {
    u16 limit;
    u32 base;
} __attribute__((packed)) GDTPointer;

void gdt_set_entry(u32 index, u32 base, u32 limit, u8 access, u8 flags);
void gdt_init();

#endif
