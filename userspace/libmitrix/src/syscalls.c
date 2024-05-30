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