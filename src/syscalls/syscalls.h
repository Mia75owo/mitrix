#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "util/types.h"

#define SYSCALLS_COUNT 64

void syscalls_init();
void syscalls_register(u32 vector, void* func);

#endif
