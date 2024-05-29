#include "syscalls.h"

#include "idt/idt.h"
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

static void syscall_print(const char* string) { klog("%s", string); }

void syscalls_init() {
    memset(syscall_handlers, 0, sizeof(syscall_handlers));

    syscall_handlers[0] = syscall_print;

    set_isr_function(0x80, handle_syscall_interrupt);
}
