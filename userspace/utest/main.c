#include "syscalls.h"
#include "stdio.h"

int _start() {
    char buf[256];
    snprintf(buf, 256, "LIBC :D %i\n", 1234) ;

    // syscall_print("userspace/utest/utest.c :)");
    syscall_print(buf);
    syscall_print_char('\n');
    syscall_exit();

    return 0;
}
