#include "syscalls.h"

#include "disk/mifs.h"
#include "events/events.h"
#include "gfx/fb_manager.h"
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

    // Remove event receiver
    if (task->shmem_events_obj != -1) {
        void* events_vaddr = shmem_get_vaddr(task->shmem_events_obj, 0);
        events_remove_receiver(events_vaddr);

        shmem_unmap(task->shmem_events_obj, 0);
        shmem_destroy(task->shmem_events_obj);
    }
    if (events_get_focused_task() == task) {
        events_focus_task(NULL);
    }

    shmem_destroy_owned_by(task_id);

    if (fb_manager_get_current_handle_id() == task->fb_handle_id) {
        // Enable GUI render loop
        Task* gui_task = task_manager_get_task(1);
        gui_task->state = TASK_STATE_RUNNING;
        gui_trigger_entire_redraw();
    }
    if (task->fb_handle_id != -1) {
        fb_manager_remove(task->fb_handle_id);
    }

    // Free userheap
    userheap_set_size(task, 0);

    // Unblock owner task
    if (task->owner_task != -1) {
        Task* owner = task_manager_get_task(task->owner_task);
        if (owner->state == TASK_STATE_BLOCKED_BY_EXEC) {
            owner->state = TASK_STATE_RUNNING;
        }
    }

    task_manager_kill_current_task();
    gfx_doublebuffering(true);
}
static void syscall_print(const char* string) { klog("%s", string); }
static void syscall_print_char(const char c) { klog("%c", c); }
static u32 syscall_get_systime() { return pit_get_tics(); }
static u32 syscall_read(u32 file_id, u8* buffer, u32 len) {
    Task* task = task_manager_get_current_task();

    if (file_id == SYSCALL_STDIN_FILE) {
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
        assert(task->files[file_index].file.addr);

        memcpy(
            buffer,
            task->files[file_index].file.addr + task->files[file_index].offset,
            len);
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
    assert(task->files[file_index].file.addr);

    memcpy(task->files[file_id].file.addr + task->files[file_id].offset, buffer,
           len);
    return len;
}
static void syscall_exec(char* file_name) {
    Task* child = create_user_task(file_name);
    child->owner_task = task_manager_get_current_task_id();
}
static void syscall_exec_blocking(char* file_name) {
    Task* task = task_manager_get_current_task();

    Task* child = create_user_task(file_name);
    child->owner_task = task_manager_get_current_task_id();

    task->state = TASK_STATE_BLOCKED_BY_EXEC;
    task_manager_schedule();
}

static u32* syscall_create_fb(u32 width, u32 height, bool double_buffering) {
    Task* task = task_manager_get_current_task();
    u32 task_id = task_manager_get_current_task_id();

    u32 object_id = shmem_create(width * height * sizeof(u32), task_id);
    u8* addr = shmem_map(object_id, task_id);
    assert(addr);

    task->fb_handle_id = fb_manager_add(object_id, addr, double_buffering, task);

    return (u32*)addr;
}
u32 last_tick = 0xFFFFFFFF;
u32 frames = 0;
u32 seconds = 0;
static void syscall_draw_fb(u32 width, u32 height) {
    Task* task = task_manager_get_current_task();

    u8* addr = fb_manager_map(task->fb_handle_id);

    // Enable interrupts during this copying, to not block timer interrupts
    asm volatile("sti");
    gfx_clone((SCREEN_X - width) / 2, (SCREEN_Y - height) / 2, width, height,
              (u32*)addr);
    gfx_display_backbuffer();


#if (defined(DEBUG) && 0)
    u32 time = pit_get_tics();
    u32 dt = time - last_tick;
    frames++;
    seconds = (time / 1000) + 1;
    serial_printf("DRAW time: %n -> (fps %n) -> (tick %n) -> (average %n)\n",
                  (u64)dt, (u64)(1000.f / (float)dt), (u64)time,
                  (u64)(frames / seconds));
    last_tick = time;
#endif
}
static void syscall_request_screen() {
    // Pause GUI render loop
    Task* gui_task = task_manager_get_task(1);
    gui_task->state = TASK_STATE_IDLE;
}
u32 syscall_get_screen_size_x() { return SCREEN_X; }
u32 syscall_get_screen_size_y() { return SCREEN_Y; }

u32 syscall_file_open(char* file_name) {
    Task* task = task_manager_get_current_task();

    i32 file_index = -1;
    for (u32 i = 0; i < TASK_MAX_FILES; i++) {
        if (task->files[i].file.addr == 0) {
            file_index = i;
            break;
        }
    }
    assert(file_index >= 0);

    FilePtr file = mifs_file(file_name);
    if (file.addr == 0) {
        return 0;
    }

    task->files[file_index].file = file;
    task->files[file_index].offset = 0;
    return file_index + 10;
}
u32 syscall_file_open_index(u32 fs_file_index) {
    Task* task = task_manager_get_current_task();

    i32 file_index = -1;
    for (u32 i = 0; i < TASK_MAX_FILES; i++) {
        if (task->files[i].file.addr == 0) {
            file_index = i;
            break;
        }
    }
    assert(file_index >= 0);

    FilePtr file = mifs_file_by_index(fs_file_index);
    if (file.addr == 0) {
        return 0;
    }

    task->files[file_index].file = file;
    task->files[file_index].offset = 0;

    return file_index + 10;
}

void syscall_file_close(u32 file_id) {
    if (file_id == 0) return;

    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    task->files[file_index].file.addr = 0;
}
u32 syscall_get_file_offset(u32 file_id) {
    if (file_id == 0) return 0;
    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    assert(task->files[file_index].file.addr);

    return task->files[file_index].offset;
}
void syscall_set_file_offset(u32 file_id, u32 offset) {
    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    assert(task->files[file_index].file.addr);

    task->files[file_index].offset = offset;
}
u32 syscall_get_file_size(u32 file_id) {
    if (file_id == 0) return 0;
    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    assert(task->files[file_index].file.addr);

    return task->files[file_index].file.size;
}
u32 syscall_get_file_count() { return mifs_get_file_count(); }
void syscall_get_file_name(u32 file_id, char* buffer, u32 buffer_length) {
    if (file_id == 0) return;
    assert(file_id >= 10);
    u32 file_index = file_id - 10;

    Task* task = task_manager_get_current_task();
    assert(task->files[file_index].file.addr);

    strncpy(buffer, task->files[file_index].file.name, buffer_length);
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

void syscall_scheduler_next() { task_manager_schedule(); }
void syscall_sleep(u32 ms) {
    Task* task = task_manager_get_current_task();
    task->state = TASK_STATE_SLEEPING;
    task->sleep_timestamp = pit_get_tics() + ms;
    task_manager_schedule();
}
void syscall_wait_for_event() {
    Task* task = task_manager_get_current_task();
    task->state = TASK_STATE_WAIT_FOR_EVENT;
    task_manager_schedule();
}

EventBuffer* syscall_create_events_buf() {
    Task* task = task_manager_get_current_task();
    u32 task_id = task_manager_get_current_task_id();

    if (task->shmem_events_obj != -1) {
        shmem_unmap(task_id, task->shmem_events_obj);
        shmem_destroy(task->shmem_events_obj);
    }

    u32 object_id = shmem_create(sizeof(EventBuffer), task_id);
    task->shmem_events_obj = object_id;

    void* kernel_vaddr = shmem_map(object_id, 0);
    assert(kernel_vaddr);
    events_add_receiver(kernel_vaddr, task, false);

    void* user_vaddr = shmem_map(object_id, task_id);
    assert(user_vaddr);
    return user_vaddr;
}

void syscalls_init() {
    memset(syscall_handlers, 0, sizeof(syscall_handlers));

    syscall_handlers[SYSCALL_EXIT] = syscall_exit;
    syscall_handlers[SYSCALL_PRINT] = syscall_print;
    syscall_handlers[SYSCALL_PRINT_CHAR] = syscall_print_char;
    syscall_handlers[SYSCALL_GET_SYSTIME] = syscall_get_systime;
    syscall_handlers[SYSCALL_READ] = syscall_read;
    syscall_handlers[SYSCALL_WRITE] = syscall_write;
    syscall_handlers[SYSCALL_EXEC] = syscall_exec;
    syscall_handlers[SYSCALL_EXEC_BLOCKING] = syscall_exec_blocking;

    syscall_handlers[SYSCALL_CREATE_FB] = syscall_create_fb;
    syscall_handlers[SYSCALL_DRAW_FB] = syscall_draw_fb;
    syscall_handlers[SYSCALL_REQUEST_SCREEN] = syscall_request_screen;
    syscall_handlers[SYSCALL_GET_SCREEN_SIZE_X] = syscall_get_screen_size_x;
    syscall_handlers[SYSCALL_GET_SCREEN_SIZE_Y] = syscall_get_screen_size_y;

    syscall_handlers[SYSCALL_FILE_OPEN] = syscall_file_open;
    syscall_handlers[SYSCALL_FILE_CLOSE] = syscall_file_close;
    syscall_handlers[SYSCALL_GET_FILE_OFFSET] = syscall_get_file_offset;
    syscall_handlers[SYSCALL_SET_FILE_OFFSET] = syscall_set_file_offset;
    syscall_handlers[SYSCALL_GET_FILE_SIZE] = syscall_get_file_size;
    syscall_handlers[SYSCALL_GET_FILE_COUNT] = syscall_get_file_count;
    syscall_handlers[SYSCALL_GET_FILE_NAME] = syscall_get_file_name;
    syscall_handlers[SYSCALL_FILE_OPEN_INDEX] = syscall_file_open_index;

    syscall_handlers[SYSCALL_GET_HEAP_START] = syscall_get_heap_start;
    syscall_handlers[SYSCALL_GET_HEAP_END] = syscall_get_heap_end;
    syscall_handlers[SYSCALL_SET_HEAP_SIZE] = syscall_set_heap_size;

    syscall_handlers[SYSCALL_SCHEDULER_NEXT] = syscall_scheduler_next;
    syscall_handlers[SYSCALL_SLEEP] = syscall_sleep;
    syscall_handlers[SYSCALL_WAIT_FOR_EVENT] = syscall_wait_for_event;

    syscall_handlers[SYSCALL_CREATE_EVENTS_BUF] = syscall_create_events_buf;

    set_isr_function(0x80, handle_syscall_interrupt);
}
