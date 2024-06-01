#include "types.h"
#include "syscalls.h"

void syscall_exit() {
    u32 ret;
    u32 syscall = SYSCALL_EXIT;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall));
}
void syscall_print(const char* str) {
    u32 ret;
    u32 syscall = SYSCALL_PRINT;
    u32 arg0 = (u32)str;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg0));
}
void syscall_print_char(const char c) {
    u32 ret;
    u32 syscall = SYSCALL_PRINT_CHAR;
    u32 arg0 = (u32)c;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg0));
}
u32 syscall_get_systime() {
    u32 ret;
    u32 syscall = SYSCALL_GET_SYSTIME;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall));
    return ret;
}
u32 syscall_read(u32 file_id, u8* buf, u32 len) {
    u32 ret;
    u32 syscall = SYSCALL_READ;
    u32 arg0 = (u32)file_id;
    u32 arg1 = (u32)buf;
    u32 arg2 = (u32)len;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg0), "c"(arg1), "d"(arg2));
    return ret;
}
u32 syscall_write(u32 file_id, u8* buf, u32 len) {
    u32 ret;
    u32 syscall = SYSCALL_WRITE;
    u32 arg0 = (u32)file_id;
    u32 arg1 = (u32)buf;
    u32 arg2 = (u32)len;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall), "b"(arg0), "c"(arg1), "d"(arg2));
    return ret;
}
u8* syscall_create_fb() {
    u32 ret;
    u32 syscall = SYSCALL_CREATE_FB;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall));
    return (u8*)ret;
}
void syscall_draw_fb() {
    u32 ret;
    u32 syscall = SYSCALL_DRAW_FB;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall));
}
void syscall_request_screen() {
    u32 ret;
    u32 syscall = SYSCALL_REQUEST_SCREEN;

    asm volatile("int $0x80" : "=a"(ret) : "a"(syscall));
}
