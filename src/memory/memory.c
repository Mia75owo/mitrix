#include "memory.h"

#include "util/debug.h"
#include "util/mem.h"
#include "util/types.h"

static u32 page_frame_min;
static u32 page_frame_max;
static u32 total_alloc;

#define NUM_PAGE_DIRS 256
#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)

u8 physical_memory_bitmap[NUM_PAGE_DIRS / 8];

static u32 page_dirs[NUM_PAGE_DIRS][1024] __attribute__((aligned(4096)));
static u8 page_dirs_used[NUM_PAGE_DIRS];

static struct multiboot_info* boot_info = 0;

#define CEIL_DIV(a, b) (((a + b) - 1) / b)

void pmm_init(u32 mem_low, u32 mem_high) {
    page_frame_min = CEIL_DIV(mem_low, 0x1000);
    page_frame_max = mem_high / 0x1000;

    total_alloc = 0;
    memset(physical_memory_bitmap, 0, sizeof(physical_memory_bitmap));
}

void memory_init(u32 mem_high, u32 physical_alloc_start) {
    initial_page_dir[0] = 0;
    invalidate(0);
    initial_page_dir[1023] = ((u32)initial_page_dir - KERNEL_START) |
                             PAGE_FLAG_PRESENT | PAGE_FLAG_WRITE;
    invalidate(0xFFFFF000);

    pmm_init(physical_alloc_start, mem_high);

    memset(page_dirs, 0, 0x1000 * NUM_PAGE_DIRS);
    memset(page_dirs_used, 0, NUM_PAGE_DIRS);
}

void invalidate(u32 vaddr) { asm volatile("invlpg %0" ::"m"(vaddr)); }

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

        klog("%[80|=]");
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

    klog("%[80|=]");
    klog("%40MEMINFO_END\n");
}
