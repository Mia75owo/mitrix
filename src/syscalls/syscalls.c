#include "syscalls.h"

#include "disk/mifs.h"
#include "events/events.h"
#include "gfx/gfx.h"
#include "gfx/gui.h"
#include "gfx/tty.h"
#include "idt/idt.h"
#include "pit/pit.h"
#include "serial/serial.h"
#include "syscalls/syscall_list.h"
#include "tasks/task_manager.h"
#include "userheap/userheap.h"
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
    Task* task = task_manager_get_current_task();
    u32 task_id = task_manager_get_current_task_id();
    shmem_destroy_owned_by(task_id);

    if (currently_drawing_task == (i32)task_id) {
        currently_drawing_task = -1;
        // Enable GUI render loop
        Task* gui_task = task_manager_get_task(1);
        gui_task->state = TASK_STATE_RUNNING;
        gui_trigger_entire_redraw();
    }

    // Free userheap
    userheap_set_size(task, 0);

    task_manager_kill_current_task();
}
static void syscall_print(const char* string) { klog("%s", string); }
static void syscall_print_char(const char c) { klog("%c", c); }
static u32 syscall_get_systime() { return pit_get_tics(); }
static u32 syscall_read(u32 file_id, u8* buffer, u32 len) {
    Task* task = task_manager_get_current_task();

    if (file_id == SYSCALL_STDIN_FILE)  {
        asm volatile("sti");
        char* user_input = gui_get_user_input(len);

        if (len <= GUI_PROMPT_SIZE) {
            memcpy(buffer, user_input, len);
            return len;
        } else {
            memcpy(buffer, user_input, GUI_PROMPT_SIZE);
            return GUI_PROMPT_SIZE;
        }
    } else {
        assert(file_id >= 10);
        u32 file_index = file_id - 10;
        assert(task->files[file_index].addr);
        
        memcpy(buffer, task->files[file_index].addr + task->files[file_index].offset, len);
        return len;
    }
}
static u32 syscall_write(u32 file_id, u8* buffer, u32 len) {
    Task* task = task_manager_get_current_task();

    if (file_id == SYSCALL_STDOUT_FILE) {
        tty_putbuf((char*)buffer, len, 0x03);
        for (u32 i = 0; i < len; i++) {
            serial_putchar(buffer[i]);
        }
        return len;
    } else if (file_id == SYSCALL_STDERR_FILE) {
        tty_putbuf((char*)buffer, len, 0x0c);
        for (u32 i = 0; i < len; i++) {
            serial_putchar(buffer[i]);
        }
        return len;
    }

    assert(file_id >= 10);
    u32 file_index = file_id - 10;
    assert(task->files[file_index].addr);

    memcpy(task->files[file_id].addr + task->files[file_id].offset, buffer, len);
    return len;
}
static u32* syscall_create_fb() {
    u32 task_id = task_manager_get_current_task_id();

    // TODO: make the size dynamic
    u32 object_id = shmem_create(800 * 600 * sizeof(u32), task_id);
    u8* addr = shmem_map(object_id, task_id);
    assert(addr);

    Task* task = task_manager_get_task(task_id);
    task->shmem_fb_obj = object_id;

    return (u32*)addr;
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
EventBuffer* syscall_create_events_buf() {
    Task* task = task_manager_get_current_task();
    u32 task_id = task_manager_get_current_task_id();

    if (task->shmem_events_obj != -1) {
        shmem_unmap(task_id, task->shmem_events_obj);
        shmem_destroy(task->shmem_events_obj);
    }

    u32 object_id = shmem_create(sizeof(EventBuffer), task_id);
    task->shmem_events_obj = task_id;

    void* kernel_vaddr = shmem_map(object_id, 0);
    assert(kernel_vaddr);
    events_add_receiver(kernel_vaddr);

    void* user_vaddr = shmem_map(object_id, task_id);
    assert(user_vaddr);
    return user_vaddr;
}

u32 syscall_file_open(char* file_name) {
    Task* task = task_manager_get_current_task();

    i32 file_index = -1;
    for (u32 i = 0; i < TASK_MAX_FILES; i++) {
        if (task->files[i].addr == 0) {
            file_index = i;
            break;
        }
    }
    assert(file_index >= 0);

    FilePtr file = mifs_file(file_name);
    if (file.addr == 0) {
        return 0;
    }

    task->files[file_index].addr = file.addr;
    task->files[file_index].offset = 0;
    task->files[file_index].size = file.size;
    return file_index + 10;
}
void syscall_file_close(u32 file_id) {
    if (file_id == 0) return;

    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    task->files[file_index].addr = 0;
}
void* syscall_get_heap_start() {
    Task* task = task_manager_get_current_task();
    return (void*)task->heap_start;
}
void* syscall_get_heap_end() {
    Task* task = task_manager_get_current_task();
    return (void*)task->heap_end;
}
void syscall_set_heap_size(u32 size) {
    Task* task = task_manager_get_current_task();
    userheap_set_size(task, size);
}
u32 syscall_get_file_offset(u32 file_id) {
    if (file_id == 0) return 0;
    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    assert(task->files[file_index].addr);

    return task->files[file_index].offset;
}
void syscall_set_file_offset(u32 file_id, u32 offset) {
    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    assert(task->files[file_index].addr);

    task->files[file_index].offset = offset;
}
u32 syscall_get_file_size(u32 file_id) {
    if (file_id == 0) return 0;
    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    assert(task->files[file_index].addr);

    return task->files[file_index].size;
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

    syscall_handlers[SYSCALL_CREATE_EVENTS_BUF] = syscall_create_events_buf;

    syscall_handlers[SYSCALL_FILE_OPEN] = syscall_file_open;
    syscall_handlers[SYSCALL_FILE_CLOSE] = syscall_file_close;

    syscall_handlers[SYSCALL_READ] = syscall_read;
    syscall_handlers[SYSCALL_WRITE] = syscall_write;

    syscall_handlers[SYSCALL_GET_HEAP_START] = syscall_get_heap_start;
    syscall_handlers[SYSCALL_GET_HEAP_END] = syscall_get_heap_end;
    syscall_handlers[SYSCALL_SET_HEAP_SIZE] = syscall_set_heap_size;

    syscall_handlers[SYSCALL_GET_FILE_OFFSET] = syscall_get_file_offset;
    syscall_handlers[SYSCALL_SET_FILE_OFFSET] = syscall_set_file_offset;
    syscall_handlers[SYSCALL_GET_FILE_SIZE] = syscall_get_file_size;

    set_isr_function(0x80, handle_syscall_interrupt);
}
