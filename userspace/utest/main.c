#include "syscalls.h"
#include "stdio.h"

int _start() {
    char buf[256];

    fgets(buf, 256, stdin);

    i32 foo = 0;
    sscanf(buf, "%d", &foo);

    printf("owo: %d\n", foo);

    syscall_exit();


    return 0;
}
