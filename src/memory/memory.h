#ifndef MEMORY_H_
#define MEMORY_H_

#include "kernel/multiboot.h"

extern u32 initial_page_dir[1024];

#define KERNEL_START 0xC0000000
#define KMEM(a) ((void*)((u32)KERNEL_START + (u32)(a)))

#define PAGE_FLAG_PRESENT (1 << 0)
#define PAGE_FLAG_WRITE   (1 << 1)

void pmm_init(u32 mem_low, u32 mem_high);

void memory_init(u32 mem_high, u32 physical_alloc_start);

void memory_set_boot_info(struct multiboot_info* in_boot_info);
void memory_print_info();

void invalidate(u32 vaddr);

#endif
