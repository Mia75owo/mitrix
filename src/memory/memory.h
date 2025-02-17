#ifndef MEMORY_H_
#define MEMORY_H_

#include "kernel/multiboot.h"

extern u32 initial_page_dir[1024];

// clang-format off
#define USER_STACK_PAGES  16

#define USER_HEAP_START   0x20000000
#define USER_STACK_BOTTOM 0xB0000000
#define USER_SHARED_MEM   0xB0000000

#define KERNEL_START      0xC0000000
#define KERNEL_GFX        0xC8000000
#define KERNEL_MALLOC     0xD0000000
#define KERNEL_RAMDISK    0xE0000000
#define KERNEL_SHARED_MEM 0xF0000000
// clang-format on

#define KMEM(a) ((void*)((u32)KERNEL_START + (u32)(a)))

#define PD_INDEX(address) ((address) >> 22)
#define PT_INDEX(address) (((address) >> 12) & 0x3FF)
#define PAGE_PHYS_ADDRESS(dir_entry) ((*dir_entry) & ~0xFFF)
#define SET_ATTRIBUTES(entry, attr) (*(entry) |= (attr))
#define CLEAR_ATTRIBUTES(entry, attr) (*(entry) &= ~(attr))
#define TEST_ATTRIBUTES(entry, attr) (*(entry) & (attr))
#define SET_FRAME(entry, attr) (*(entry) = (*(entry) & ~0x7FFFF000) | (address))

#define REC_PAGEDIR ((u32*)0xFFFFF000)
#define REC_PAGETABLE(i) ((u32*)(0xFFC00000 + ((i) << 12)))
#define P_PHYS_ADDR(x) ((x) & ~0xFFF)

typedef u32 pt_entry;
typedef u32 pd_entry;
typedef u32 physical_address;
typedef u32 virtual_address;

extern u32 memory_num_vpages;

// clang-format off
typedef enum {
    PTE_PRESENT       = 0x01,
    PTE_READ_WRITE    = 0x02,
    PTE_USER          = 0x04,
    PTE_WRITE_THORUGH = 0x08,
    PTE_CACHE_DISABLE = 0x10,
    PTE_ACCESSED      = 0x20,
    PTE_DIRTY         = 0x40,
    PTE_PAT           = 0x80,
    PTE_GLOBAL        = 0x100,
    PTE_OWNER         = 0x200,
    PTE_FRAME         = 0x7FFFF000,
} PAGE_TABLE_FLAGS;
// clang-format on

// clang-format off
typedef enum {
    PDE_PRESENT       = 0x01,
    PDE_READ_WRITE    = 0x02,
    PDE_USER          = 0x04,
    PDE_WRITE_THORUGH = 0x08,
    PDE_CACHE_DISABLE = 0x10,
    PDE_ACCESSED      = 0x20,
    PDE_DIRTY         = 0x40,
    PDE_PAGE_SIZE     = 0x80,       // 0 = 4KB page, 1 = 4MB page
    PDE_GLOBAL        = 0x100,
    PDE_PAT           = 0x2000,
    PDE_FRAME         = 0x7FFFF000,
} PAGE_DIR_FLAGS;
// clang-format on

void memory_init(u32 mem_high, u32 physical_alloc_start);

void memory_set_boot_info(struct multiboot_info* in_boot_info);
void memory_print_info();

u32* memory_alloc_page_dir();
void memory_free_page_dir(u32* page_dir);

void memory_map_page(u32 virt_addr, u32 phys_addr, u32 flags);
u32 memory_unmap_page(u32 virt_addr);

u32* memory_get_current_page_dir();
void memory_change_page_dir(u32* pd);

bool memory_is_valid_vaddr(u32 addr);

void invalidate(u32 vaddr);

#endif
