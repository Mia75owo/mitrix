#include "syscalls.h"
#include "stdio.h"

int _start() {
    fprintf(stderr, "hi");
    syscall_exit();


    return 0;
}
