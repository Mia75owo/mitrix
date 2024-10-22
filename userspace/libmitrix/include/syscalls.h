#ifndef SYSCALLS_H_
#define SYSCALLS_H_

#include "../../../src/syscalls/syscall_list.h"
#include "types.h"
#include "events.h"

void syscall_exit();
void syscall_print(const char* str);
void syscall_print_char(const char c);
u32 syscall_get_systime();
u32 syscall_read(u32 file_id, u8* buf, u32 len);
u32 syscall_write(u32 file_id, u8* buf, u32 len);
void syscall_exec(char* file_name);
void syscall_exec_blocking(char* file_name);
void syscall_exec_detached(char* file_name);
u32 syscall_owner_task_depth();

u32* syscall_create_fb(u32 width, u32 height);
void syscall_draw_fb();
u32 syscall_get_screen_size_x();
u32 syscall_get_screen_size_y();

u32 syscall_file_open(char* file_name);
void syscall_file_close(u32 file_id);
u32 syscall_get_file_offset(u32 file_id);
void syscall_set_file_offset(u32 file_id, u32 offset);
u32 syscall_get_file_size(u32 file_id);
u32 syscall_get_file_count();
void syscall_get_file_name(u32 file_id, char* buffer, u32 buffer_length);
u32 syscall_file_open_index(u32 fs_file_index);

void* syscall_get_heap_start();
void* syscall_get_heap_end();
void syscall_set_heap_size(u32 size);

void syscall_scheduler_next();
void syscall_sleep(u32 ms);
void syscall_wait_for_event();
bool syscall_request_focus();

EventBuffer* syscall_create_events_buf();

#endif
