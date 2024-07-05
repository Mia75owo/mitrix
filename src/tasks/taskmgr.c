#include "taskmgr.h"

#include "memory/memory.h"
#include "pit/pit.h"
#include "util/debug.h"
#include "util/mem.h"

static Task tasks[NUM_TASKS];
static u32 current_task;

void taskmgr_init() {
    memset(tasks, 0, sizeof(tasks));
    current_task = 0;

    tasks[current_task].state = TASK_STATE_RUNNING;
    tasks[current_task].page_dir = memory_get_current_page_dir();
    tasks[0].shmem_pool.vaddr_start = KERNEL_SHARED_MEM;
}

static i32 get_free_task_slot() {
    for (u32 i = 0; i < NUM_TASKS; i++) {
        if (tasks[i].state == TASK_STATE_DEAD) {
            return i;
        }
    }
    return -1;
}

// Task creation

TaskHandle taskmgr_create_kernel_task(void entrypoint()) {
    i32 slot = get_free_task_slot();
    if (slot == -1) {
        return -1;
    }
    task_kernel_create(&tasks[slot], entrypoint);
    tasks[slot].state = TASK_STATE_RUNNING;
    return slot;
}
TaskHandle taskmgr_create_user_task(char* elf_file_name) {
    i32 slot = get_free_task_slot();
    if (slot == -1) {
        return -1;
    }
    task_user_create(&tasks[slot], elf_file_name);
    tasks[slot].state = TASK_STATE_RUNNING;
    return slot;
}

// Task* <> TaskHandle

Task* taskmgr_handle_to_pointer(TaskHandle handle) {
    if (handle < 0 || handle >= NUM_TASKS) {
        return NULL;
    }
    return &tasks[handle];
}
TaskHandle taskmgr_handle_from_pointer(Task* ptr) {
    i32 handle = -1;
    for (u32 i = 0; i < NUM_TASKS; i++) {
        if (&tasks[i] == ptr) {
            handle = i;
            break;
        }
    }
    return handle;
}

// Task getters

TaskHandle taskmgr_get_current_task() { return current_task; }

TaskHandle taskmgr_get_kernel_task() { return 0; }

// Task management functions

void taskmgr_kill_task(TaskHandle handle) {
    assert(handle >= 0 && handle < NUM_TASKS);
    task_kill(&tasks[handle]);
}
void taskmgr_set_state(TaskHandle handle, TaskState state) {
    assert(handle >= 0 && handle < NUM_TASKS);
    tasks[handle].state = state;
}
void taskmgr_focus_task(TaskHandle handle) {
    assert(handle >= 0 && handle < NUM_TASKS);
}
void taskmgr_enable_task(TaskHandle handle) {
    assert(handle >= 0 && handle < NUM_TASKS);
    tasks[handle].state = TASK_STATE_RUNNING;
}

// Switch to next task

static u32 tasks_choose_next() {
    i32 index = current_task;

    for (u32 i = 0; i < NUM_TASKS; i++) {
        index++;
        if (index >= NUM_TASKS) {
            index = 0;
        }

        // Skip unused slots
        if (tasks[index].state == TASK_STATE_DEAD) {
            continue;
        }

        // Wake up task from sleeping
        if (tasks[index].state == TASK_STATE_SLEEPING &&
            tasks[index].sleep_timestamp <= pit_get_tics()) {
            tasks[index].state = TASK_STATE_RUNNING;
        }
        // Wake up task for event
        if (tasks[index].state == TASK_STATE_WAIT_FOR_EVENT &&
            tasks[index].pending_events) {
            tasks[index].state = TASK_STATE_RUNNING;
            tasks[index].pending_events = false;
        }

        if (tasks[index].state == TASK_STATE_RUNNING) {
            return index;
        }
    }

    return 0;
}

// Info getters

bool taskmgr_is_current_task_alive() {
    return tasks[current_task].state != TASK_STATE_DEAD;
}

void taskmgr_schedule() {
    u32 index = tasks_choose_next();
    if (current_task == (u32)index) return;

    Task* old = &tasks[current_task];
    Task* new = &tasks[index];

    current_task = index;

    task_switch(old, new);
}
