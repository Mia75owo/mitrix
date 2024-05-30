#include "task_manager.h"

#include "memory/memory.h"
#include "util/mem.h"

static Task tasks[TASKS_COUNT];

static u32 current_task;
static u32 num_tasks;

void task_manager_init() {
    num_tasks = 1;
    current_task = 0;
    memset(tasks, 0, sizeof(tasks));

    tasks[current_task].state = TASK_STATE_RUNNING;
    tasks[current_task].page_dir = memory_get_current_page_dir();
}

static i32 get_free_task_slot() {
    for (u32 i = 0; i < TASKS_COUNT; i++) {
        if (tasks[i].state == TASK_STATE_DEAD) {
            return i;
        }
    }
    return -1;
}

static u32 tasks_choose_next() {
    i32 index = current_task;

    for (u32 i = 0; i < TASKS_COUNT; i++) {
        index++;
        index %= TASKS_COUNT;

        if (tasks[index].state == TASK_STATE_RUNNING) {
            return index;
        }
    }

    return 0;
}

void task_manager_schedule() {
    i32 index = tasks_choose_next();

    Task* old = &tasks[current_task];
    Task* new = &tasks[index];

    current_task = index;

    task_switch(old, new);
}

Task* create_kernel_task(void entrypoint()) {
    i32 slot = get_free_task_slot();
    if (slot == -1) {
        return NULL;
    }
    task_kernel_create(&tasks[slot], entrypoint);
    tasks[slot].state = TASK_STATE_RUNNING;
    return &tasks[slot];
}
Task* create_user_task(char* elf_file_name) {
    i32 slot = get_free_task_slot();
    if (slot == -1) {
        return NULL;
    }
    task_user_create(&tasks[slot], elf_file_name);
    tasks[slot].state = TASK_STATE_RUNNING;
    return &tasks[slot];
}

void task_manager_kill_current_task() {
    task_kill(&tasks[current_task]);
    num_tasks--;
}
bool task_manager_current_task_alive() {
    return tasks[current_task].state == TASK_STATE_RUNNING;
}