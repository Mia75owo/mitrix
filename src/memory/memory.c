#include "memory.h"

#include "memory/pmm.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/types.h"

#define NUM_PAGE_DIRS 256
static u32 page_dirs[NUM_PAGE_DIRS][1024] __attribute__((aligned(4096)));
static u8 page_dir_used[NUM_PAGE_DIRS];

u32 memory_num_vpages;

static struct multiboot_info* boot_info = 0;

void memory_init(u32 mem_high, u32 physical_alloc_start) {
    memory_num_vpages = 0;

    initial_page_dir[0] = 0;
    invalidate(0);

    initial_page_dir[1023] =
        ((u32)initial_page_dir - KERNEL_START) | PTE_PRESENT | PTE_READ_WRITE;
    invalidate(0xFFFFF000);

    pmm_init(physical_alloc_start, mem_high);

    memset(page_dirs, 0, 0x1000 * NUM_PAGE_DIRS);
    memset(page_dir_used, 0, NUM_PAGE_DIRS);
}

// Remove from Translate Lookaside Buffer
void invalidate(u32 vaddr) { asm volatile("invlpg %0" ::"m"(vaddr)); }

u32* memory_get_current_page_dir() {
    u32 pd;
    asm volatile("mov %%cr3, %0" : "=r"(pd));
    pd += KERNEL_START;
    return (u32*)pd;
}

void memory_change_page_dir(u32* pd) {
    pd = (u32*)(((u32)pd) - KERNEL_START);
    asm volatile(
        "mov %0, %%eax\n"
        "mov %%eax, %%cr3\n" ::"m"(pd));
}

void memory_map_page(u32 virt_addr, u32 phys_addr, u32 flags) {
    u32* prev_page_dir = 0;

    if (virt_addr >= KERNEL_START) {
        prev_page_dir = memory_get_current_page_dir();

        if (prev_page_dir == initial_page_dir) {
            memory_change_page_dir(initial_page_dir);
        }
    }

    u32* page_dir = REC_PAGEDIR;
    u32* pt = REC_PAGETABLE(PD_INDEX(virt_addr));

    if (!(page_dir[PD_INDEX(virt_addr)] & PTE_PRESENT)) {
        u32 pt_paddr = pmm_alloc_pageframe();

        page_dir[PD_INDEX(virt_addr)] =
            pt_paddr | PTE_PRESENT | PTE_READ_WRITE | PTE_OWNER | flags;
        invalidate(virt_addr);

        for (u32 i = 0; i < 1024; i++) {
            pt[i] = 0;
        }
    }

    pt[PT_INDEX(virt_addr)] = phys_addr | PTE_PRESENT | flags;
    memory_num_vpages++;
    invalidate(virt_addr);

    if (prev_page_dir != 0) {
        sync_page_dirs();

        if (prev_page_dir != initial_page_dir) {
            memory_change_page_dir(prev_page_dir);
        }
    }
}

u32 memory_unmap_page(u32 virt_addr) {
    u32* prev_page_dir = 0;

    if (virt_addr >= KERNEL_START) {
        prev_page_dir = memory_get_current_page_dir();

        if (prev_page_dir != initial_page_dir) {
            memory_change_page_dir(initial_page_dir);
        }
    }

    u32* page_dir = REC_PAGEDIR;

    u32 pd_entry = page_dir[PD_INDEX(virt_addr)];
    assert_msg(pd_entry & PTE_PRESENT,
               "Tried to free page from a non presetn page table!");

    u32* pt = REC_PAGETABLE(PD_INDEX(virt_addr));

    u32 pte = pt[PT_INDEX(virt_addr)];
    assert_msg(pte & PTE_PRESENT, "Tried to free non present page!");

    pt[PT_INDEX(virt_addr)] = 0;
    memory_num_vpages--;

    bool remove = true;
    for (u32 i = 0; i < 1024; i++) {
        if (pt[i] & PTE_PRESENT) {
            remove = false;
            break;
        }
    }

    if (remove) {
        u32 pde = page_dir[PD_INDEX(virt_addr)];
        if (pde & PTE_OWNER) {
            u32 pt_paddr = P_PHYS_ADDR(pde);

            pmm_free_pageframe(pt_paddr);
            page_dir[PD_INDEX(virt_addr)] = 0;
        }
    }

    invalidate(virt_addr);

    if (pte & PTE_OWNER) {
        pmm_free_pageframe(P_PHYS_ADDR(pte));
    }

    if (prev_page_dir != 0) {
        sync_page_dirs();
        if (prev_page_dir != initial_page_dir) {
            memory_change_page_dir(prev_page_dir);
        }
    }

    return pte;
}

void sync_page_dirs() {
    for (u32 i = 0; i < NUM_PAGE_DIRS; i++) {
        if (page_dir_used[i]) {
            u32* page_dir = page_dirs[i];

            for (u32 i = 768; i < 1023; i++) {
                page_dir[i] = initial_page_dir[i] & ~PTE_OWNER;
            }
        }
    }
}

u32* memory_alloc_page_dir() {
    for (u32 i = 0; i < NUM_PAGE_DIRS; i++) {
        if (!page_dir_used[i]) {
            page_dir_used[i] = true;

            u32* page_dir = page_dirs[i];
            memset(page_dir, 0, 0x1000);

            // user page tables
            for (u32 i = 0; i < 768; i++) {
                page_dir[i] = 0;
            }

            // kernel
            for (u32 i = 768; i < 1023; i++) {
                page_dir[i] = initial_page_dir[i] & ~PTE_OWNER;
            }

            // resursive mapping
            page_dir[1023] =
                (((u32)page_dir) - KERNEL_START) | PTE_PRESENT | PTE_READ_WRITE;
            return page_dir;
        }
    }

    assert_msg(false, "No page dirs left!");
    return 0;
}

void memory_free_page_dir(u32* page_dir) {
    u32* prev_pagedir = memory_get_current_page_dir();
    memory_change_page_dir(page_dir);

    u32 pagedir_index = ((u32)page_dir) - ((u32)page_dirs);
    pagedir_index /= 4096;

    u32* pd = REC_PAGEDIR;
    for (u32 i = 0; i < 768; i++) {
        u32 pde = pd[i];
        if (pde == 0) {
            continue;
        }

        u32* ptable = REC_PAGETABLE(i);
        for (u32 j = 0; j < 1024; j++) {
            u32 pte = ptable[j];

            if (pte & PTE_OWNER) {
                pmm_free_pageframe(P_PHYS_ADDR(pte));
            }
        }
        memset(ptable, 0, 4096);

        if (pde & PTE_OWNER) {
            pmm_free_pageframe(P_PHYS_ADDR(pde));
        }
        pd[i] = 0;
    }

    page_dir_used[pagedir_index] = 0;
    memory_change_page_dir(prev_pagedir);
}

void memory_set_boot_info(struct multiboot_info* in_boot_info) {
    boot_info = KMEM(in_boot_info);
}

void memory_print_info() {
    if (boot_info == 0) return;

    klog("\n%40MEMINFO:\n");
    for (u32 i = 0; i < boot_info->mmap_length;
         i += sizeof(multiboot_mmap_entry)) {
        multiboot_mmap_entry* mmmt =
            (multiboot_mmap_entry*)KMEM(boot_info->mmap_addr + i);

        klog("%[50|=]");
        klog(
            "%0fAddr: (%0C%x%0f|%0C%x%0f) | Len: (%0C%x%0f|%0C%x%0f)\nSize: "
            "%0C%x %0f| Type: ",
            (u64)mmmt->addr_low, (u64)mmmt->addr_high, (u64)mmmt->len_low,
            (u64)mmmt->len_high, (u64)mmmt->size);

        switch (mmmt->type) {
            case MULTIBOOT_MEMORY_AVAILABLE: {
                klog("%0Aavailable\n");
            } break;
            case MULTIBOOT_MEMORY_RESERVED: {
                klog("%04reserved\n");
            } break;
            case MULTIBOOT_MEMORY_ACPI_RECLAIMABLE: {
                klog("%0Ereclaimable\n");
            } break;
            case MULTIBOOT_MEMORY_NVC: {
                klog("%0Envc\n");
            } break;
            case MULTIBOOT_MEMORY_BADRAM: {
                klog("%04badram\n");
            } break;
        }
    }

    klog("%[50|=]");
    klog("%40MEMINFO_END\n");
}
