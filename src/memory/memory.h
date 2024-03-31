#ifndef MEMORY_H_
#define MEMORY_H_

#include "kernel/multiboot.h"

void memory_init(struct multiboot_info* in_boot_info);
void memory_print_info();

#endif
