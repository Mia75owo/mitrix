#include "syscalls.h"

#include "gfx/tty.h"
#include "idt/idt.h"
#include "pit/pit.h"
#include "syscalls/syscall_list.h"
#include "tasks/task_manager.h"
#include "util/debug.h"
#include "util/mem.h"

void* syscall_handlers[SYSCALLS_COUNT];

static void handle_syscall_interrupt(CPUState* frame) {
    u32 index = frame->eax;
    void* handler = syscall_handlers[index];

    if (!handler) {
        klog("Invalid syscall: %n\n", (u64)index);
        return;
    }

    int ret;
    asm volatile(
        "push %1 \n"
        "push %2 \n"
        "push %3 \n"
        "push %4 \n"
        "push %5 \n"
        "call *%6 \n"
        "pop %%ebx \n"
        "pop %%ebx \n"
        "pop %%ebx \n"
        "pop %%ebx \n"
        "pop %%ebx \n"
        : "=a"(ret)
        : "r"(frame->edi), "r"(frame->esi), "r"(frame->edx), "r"(frame->ecx),
          "r"(frame->ebx), "r"(handler));
    frame->eax = ret;
}

static void syscall_exit() { task_manager_kill_current_task(); }
static void syscall_print(const char* string) { klog("%s", string); }
static void syscall_print_char(const char c) { klog("%c", c); }
static u32 syscall_get_systime() { return pit_get_tics(); }
static void syscall_read(u32 file_id, u8* buffer, u32 len) { 
    assert_msg(file_id <= SYSCALL_STDERR_FILE, "SYSCALL_READ: only user io allowed");
    if (file_id != SYSCALL_STDIN_FILE) {
        return;
    }

    // TODO: stdin
}
static void syscall_write(u32 file_id, u8* buffer, u32 len) {
    assert_msg(file_id <= 2, "SYSCALL_READ: only user io allowed");

    if (file_id == SYSCALL_STDOUT_FILE) {
        tty_putbuf((char*)buffer, len, 0x03);
    } else if (file_id == SYSCALL_STDERR_FILE) {
        tty_putbuf((char*)buffer, len, 0x0c);
    }
}

void syscalls_init() {
    memset(syscall_handlers, 0, sizeof(syscall_handlers));

    syscall_handlers[SYSCALL_EXIT] = syscall_exit;
    syscall_handlers[SYSCALL_PRINT] = syscall_print;
    syscall_handlers[SYSCALL_PRINT_CHAR] = syscall_print_char;
    syscall_handlers[SYSCALL_GET_SYSTIME] = syscall_get_systime;

    syscall_handlers[SYSCALL_READ] = syscall_read;
    syscall_handlers[SYSCALL_WRITE] = syscall_write;

    set_isr_function(0x80, handle_syscall_interrupt);
}
