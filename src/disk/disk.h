#ifndef DISK_H_
#define DISK_H_

#include "kernel/multiboot.h"
#include "util/types.h"

typedef struct {
    u32 size;
    u32 phys_addr;
} Ramdisk;

void disk_init(struct multiboot_info* boot_info);
void disk_map();
void disk_init_fs();

#endif
