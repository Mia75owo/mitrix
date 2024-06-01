#include "stdio.h"
#include "syscalls.h"

int _start() {
    // char buf[256];

    // fgets(buf, 256, stdin);
    // i32 foo = 0;
    // sscanf(buf, "%d", &foo);
    // printf("owo: %d\n", foo);

    u32* addr = (u32*)syscall_create_fb();
    printf("fb_addr: %x\n", addr);

    syscall_request_screen();

    for (u32 i = 0; i < 600 * 800; i++) {
        addr[i] = 0xFFFFFFFF;
    }
    for (u32 i = 0; i < 20; i++) {
        syscall_draw_fb();
    }

    syscall_exit();

    return 0;
}
