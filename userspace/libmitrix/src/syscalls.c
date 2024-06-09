#include "syscalls.h"

#include "types.h"

void syscall_exit() { asm volatile("int $0x80" ::"a"(SYSCALL_EXIT)); }
void syscall_print(const char* str) {
    asm volatile("int $0x80" ::"a"(SYSCALL_PRINT), "b"(str));
}
void syscall_print_char(const char c) {
    asm volatile("int $0x80" ::"a"(SYSCALL_PRINT_CHAR), "b"(c));
}
u32 syscall_get_systime() {
    u32 ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(SYSCALL_GET_SYSTIME));
    return ret;
}
u32 syscall_read(u32 file_id, u8* buf, u32 len) {
    u32 ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(SYSCALL_READ), "b"(file_id), "c"(buf), "d"(len));
    return ret;
}
u32 syscall_write(u32 file_id, u8* buf, u32 len) {
    u32 ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(SYSCALL_WRITE), "b"(file_id), "c"(buf), "d"(len));
    return ret;
}
u32* syscall_create_fb(u32 width, u32 height, bool double_buffering) {
    u32 ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(SYSCALL_CREATE_FB), "b"(width), "c"(height),
                   "d"(double_buffering));
    return (u32*)ret;
}
void syscall_draw_fb(u32 width, u32 height) {
    asm volatile("int $0x80" ::"a"(SYSCALL_DRAW_FB), "b"(width), "c"(height));
}
void syscall_request_screen() {
    asm volatile("int $0x80" ::"a"(SYSCALL_REQUEST_SCREEN));
}
EventBuffer* syscall_create_events_buf() {
    u32 ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(SYSCALL_CREATE_EVENTS_BUF));
    return (void*)ret;
}
u32 syscall_file_open(char* file_name) {
    u32 ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(SYSCALL_FILE_OPEN), "b"(file_name));
    return ret;
}
void syscall_file_close(u32 file_id) {
    asm volatile("int $0x80" ::"a"(SYSCALL_FILE_CLOSE), "b"(file_id));
}
void* syscall_get_heap_start() {
    u32 ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(SYSCALL_GET_HEAP_START));
    return (void*)ret;
}
void* syscall_get_heap_end() {
    u32 ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(SYSCALL_GET_HEAP_END));
    return (void*)ret;
}
void syscall_set_heap_size(u32 size) {
    asm volatile("int $0x80" ::"a"(SYSCALL_SET_HEAP_SIZE), "b"(size));
}
u32 syscall_get_file_offset(u32 file_id) {
    u32 ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(SYSCALL_GET_FILE_OFFSET), "b"(file_id));
    return ret;
}
void syscall_set_file_offset(u32 file_id, u32 offset) {
    asm volatile("int $0x80" ::"a"(SYSCALL_SET_FILE_OFFSET), "b"(file_id),
                 "c"(offset));
}
u32 syscall_get_file_size(u32 file_id) {
    u32 ret;
    asm volatile("int $0x80"
                 : "=a"(ret)
                 : "a"(SYSCALL_GET_FILE_SIZE), "b"(file_id));
    return ret;
}
u32 syscall_get_screen_size_x() {
    u32 ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(SYSCALL_GET_SCREEN_SIZE_X));
    return ret;
}
u32 syscall_get_screen_size_y() {
    u32 ret;
    asm volatile("int $0x80" : "=a"(ret) : "a"(SYSCALL_GET_SCREEN_SIZE_Y));
    return ret;
}
void syscall_scheduler_next() {
    asm volatile("int $0x80" ::"a"(SYSCALL_SCHEDULER_NEXT));
}
void syscall_wait_for_event() {
    asm volatile("int $0x80" ::"a"(SYSCALL_WAIT_FOR_EVENT));
}
