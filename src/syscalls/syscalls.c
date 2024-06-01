#include "syscalls.h"

#include "gfx/gfx.h"
#include "gfx/gui.h"
#include "gfx/tty.h"
#include "idt/idt.h"
#include "pit/pit.h"
#include "syscalls/syscall_list.h"
#include "tasks/task_manager.h"
#include "util/debug.h"
#include "util/mem.h"

static i32 currently_mapped_fb = -1;
static u8* currently_mapped_fb_addr = 0;
static i32 currently_drawing_task = -1;
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

static void syscall_exit() { 
    u32 task_id = task_manager_get_current_task_id();

    if (currently_drawing_task == (i32)task_id) {
        currently_drawing_task = -1;
        // Enable GUI render loop
        Task* gui_task = task_manager_get_task(1);
        gui_task->state = TASK_STATE_RUNNING;
        gui_trigger_entire_redraw();
    }

    task_manager_kill_current_task();
}
static void syscall_print(const char* string) { klog("%s", string); }
static void syscall_print_char(const char c) { klog("%c", c); }
static u32 syscall_get_systime() { return pit_get_tics(); }
static u32 syscall_read(u32 file_id, u8* buffer, u32 len) {
    assert_msg(file_id <= SYSCALL_STDERR_FILE,
               "SYSCALL_READ: only user io allowed");
    if (file_id != SYSCALL_STDIN_FILE) {
        return 0;
    }
    // TODO: stdin
    asm volatile("sti");
    char* user_input = gui_get_user_input(len);

    if (len <= GUI_PROMPT_SIZE) {
        memcpy(buffer, user_input, len);
        return len;
    } else {
        memcpy(buffer, user_input, GUI_PROMPT_SIZE);
        return GUI_PROMPT_SIZE;
    }
}
static u32 syscall_write(u32 file_id, u8* buffer, u32 len) {
    assert_msg(file_id <= 2, "SYSCALL_READ: only user io allowed");

    if (file_id == SYSCALL_STDOUT_FILE) {
        tty_putbuf((char*)buffer, len, 0x03);
    } else if (file_id == SYSCALL_STDERR_FILE) {
        tty_putbuf((char*)buffer, len, 0x0c);
    }

    return len;
}
static u8* syscall_create_fb() {
    u32 task_id = task_manager_get_current_task_id();

    // TODO: make the size dynamic
    u32 object_id = shmem_create(800 * 600 * sizeof(u32), task_id);
    u8* addr = shmem_map(object_id, task_id);
    assert(addr);

    Task* task = task_manager_get_task(task_id);
    task->shmem_fb_obj = object_id;

    return addr;
}
static void syscall_draw_fb() {
    Task* task = task_manager_get_current_task();
    i32 object_id = task->shmem_fb_obj;
    if (object_id == -1) {
        return;
    }
    if (currently_mapped_fb != object_id) {
        if (currently_mapped_fb != -1) {
            shmem_unmap(currently_mapped_fb, 0);
        }
        currently_mapped_fb = object_id;
        currently_mapped_fb_addr = shmem_map(object_id, 0);
    }

    klog("render %n\n", pit_get_tics());
    gfx_clone((u32*)currently_mapped_fb_addr);
}
static void syscall_request_screen() {
    Task* task = task_manager_get_current_task();
    u32 task_id = task_manager_get_current_task_id();

    i32 object_id = task->shmem_fb_obj;
    if (object_id == -1) {
        return;
    }
    if (currently_drawing_task != -1) {
        return;
    }

    currently_drawing_task = task_id;

    // Pause GUI render loop
    Task* gui_task = task_manager_get_task(1);
    gui_task->state = TASK_STATE_IDLE;
}

void syscalls_init() {
    memset(syscall_handlers, 0, sizeof(syscall_handlers));

    syscall_handlers[SYSCALL_EXIT] = syscall_exit;
    syscall_handlers[SYSCALL_PRINT] = syscall_print;
    syscall_handlers[SYSCALL_PRINT_CHAR] = syscall_print_char;
    syscall_handlers[SYSCALL_GET_SYSTIME] = syscall_get_systime;

    syscall_handlers[SYSCALL_CREATE_FB] = syscall_create_fb;
    syscall_handlers[SYSCALL_DRAW_FB] = syscall_draw_fb;
    syscall_handlers[SYSCALL_REQUEST_SCREEN] = syscall_request_screen;

    syscall_handlers[SYSCALL_READ] = syscall_read;
    syscall_handlers[SYSCALL_WRITE] = syscall_write;

    set_isr_function(0x80, handle_syscall_interrupt);
}
