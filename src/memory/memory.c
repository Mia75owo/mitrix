#include "memory.h"

#include "util/types.h"
#include "util/debug.h"

static struct multiboot_info* boot_info = 0;

void memory_init(struct multiboot_info* in_boot_info) {
    boot_info = in_boot_info;

}

void memory_print_info() {
    if (boot_info == 0) return;

    klog("\n%40MEMINFO:\n");
    for (u16 i = 0; i < boot_info->mmap_length; i += sizeof(multiboot_mmap_entry)) {
        multiboot_mmap_entry *mmmt = (multiboot_mmap_entry*)(boot_info->mmap_addr + i);

        klog("================================================================================");
        klog("%0fAddr: (%0C%x%0f|%0C%x%0f) | Len: (%0C%x%0f|%0C%x%0f)\nSize: %0C%x %0f| Type: ",
             (u64)mmmt->addr_low, (u64)mmmt->addr_high, (u64)mmmt->len_low, (u64)mmmt->len_high, (u64)mmmt->size);

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

    klog("================================================================================");
    klog("%40MEMINFO_END\n");
}
