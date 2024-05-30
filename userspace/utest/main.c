#include "syscalls.h"

int _start() {
    syscall_print("userspace/utest/utest.c :)");
    syscall_print_char('\n');
    syscall_exit();

    return 0;
}
