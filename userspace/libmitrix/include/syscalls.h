#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "../../../src/syscalls/syscall_list.h"
#include "types.h"

void syscall_exit();
void syscall_print(const char* str);
void syscall_print_char(const char c);
u32 syscall_get_systime();
u32 syscall_read(u32 file_id, u8* buf, u32 len);
u32 syscall_write(u32 file_id, u8* buf, u32 len);
u8* syscall_create_fb();
void syscall_draw_fb();
void syscall_request_screen();
void* syscall_create_events_buf();

#endif
