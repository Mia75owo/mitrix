#ifndef KMALLOC_H_
#define KMALLOC_H_

#include "util/types.h"

void kmalloc_init();

u8* kmalloc(u32 size);
void kfree(u8* addr);
void kmalloc_print_info();

#endif
