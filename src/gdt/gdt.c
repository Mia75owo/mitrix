#include "gdt.h"
#include "tasks/tss.h"

void gdt_load(u32 pointer);
extern TSS tss;
extern u32 tss_size;

static GDTEntry gdt_entries[GDT_ENTRIES_COUNT];
static GDTPointer gdt_pointer;

void gdt_set_entry(u32 index, u32 base, u32 limit, u8 access, u8 flags) {
    gdt_entries[index].base_low = (base & 0xFFFF);
    gdt_entries[index].base_mid = (base >> 16) & 0xFF;
    gdt_entries[index].base_high = (base >> 24) & 0xFF;

    gdt_entries[index].limit_low = (limit & 0xFFFF);
    gdt_entries[index].limit = (limit >> 16) & 0x0F;

    gdt_entries[index].flags = flags;
    gdt_entries[index].access = access;
}

void gdt_init() {
    gdt_pointer.limit = GDT_ENTRIES_COUNT * 8 - 1;
    gdt_pointer.base = (u32)&gdt_entries;

    gdt_set_entry(0, 0, 0, 0, 0);
    gdt_set_entry(1, 0, 0xFFFFFFFF, 0x9A, 0b1100);
    gdt_set_entry(2, 0, 0xFFFFFFFF, 0x92, 0b1100);
    gdt_set_entry(3, 0, 0xFFFFFFFF, 0xFA, 0b1100);
    gdt_set_entry(4, 0, 0xFFFFFFFF, 0xF2, 0b1100);

    gdt_set_entry(5, (u32)&tss, tss_size, 0x89, 0b1100);

    gdt_load((u32)&gdt_pointer);
}
