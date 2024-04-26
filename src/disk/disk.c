#include "disk.h"

#include "disk/mifs.h"
#include "memory/memory.h"
#include "util/debug.h"
#include "util/mem.h"

bool is_ramdisk = true;
Ramdisk ramdisk;

void disk_init(struct multiboot_info* boot_info) {
    memset(&ramdisk, 0, sizeof(Ramdisk));

    u32 mod_count = boot_info->mods_count;
    if (mod_count > 0) {
        klog("Found ramdisk\n");

        u32 phys_start = *(u32*)(boot_info->mods_addr);
        u32 phys_end = *(u32*)(boot_info->mods_addr + 4);

        klog("RAMDISK: addr %x\n", (u64)phys_start);
        klog("RAMDISK: size %n\n", (u64)(phys_end - phys_start));

        ramdisk.phys_addr = phys_start;
        ramdisk.size = phys_end - phys_start;
    }
}

void disk_map() {
    u32 pages_needed = CEIL_DIV(ramdisk.size, 0x1000);

    for (u32 i = 0; i < pages_needed; i++) {
        memory_map_page(KERNEL_RAMDISK + i * 0x1000,
                        ramdisk.phys_addr + i * 0x1000, 0);
    }
}

void disk_init_fs() {
    mifs_init((u8*)KERNEL_RAMDISK, (u8*)(KERNEL_RAMDISK + ramdisk.size));
}
