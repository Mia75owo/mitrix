#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "../../../src/syscalls/syscall_list.h"
#include "types.h"

void syscall_exit();
void syscall_print(const char* str);
void syscall_print_char(const char c);
u32 syscall_get_systime();

#endif
