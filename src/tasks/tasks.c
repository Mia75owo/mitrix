#include "tasks.h"

#include "memory/memory.h"
#include "tasks/tss.h"
#include "util/debug.h"
#include "util/mem.h"

extern void isr_exit();
extern void switch_context(Task* old, Task* next);

static Task* tasks[TASKS_COUNT];
static Task initial_task;  // initial kernel task
static u32 num_tasks;
static i32 current_task;

void tasks_init() {
    num_tasks = 1;
    current_task = 0;
    memset(tasks, 0, sizeof(tasks));

    tasks[current_task] = &initial_task;
    tasks[current_task]->page_dir = memory_get_current_page_dir();
}

void tasks_add_task(Task* task) {
    assert_msg(num_tasks <= TASKS_COUNT, "MAX TASK SIZE EXEEDED");
    tasks[num_tasks++] = task;
}

static i32 tasks_choose_next() {
    i32 index = current_task;

    for (u32 i = 0; i < TASKS_COUNT; i++) {
        index++;
        index %= TASKS_COUNT;

        if (tasks[index] != 0) {
            return index;
        }
    }

    return 0;
}

void tasks_schedule() {
    i32 index = tasks_choose_next();

    Task* next = tasks[index];
    Task* old = tasks[current_task];

    current_task = index;

    tss_update_esp0((u32)next->stack);
    switch_context(old, next);
}

void task_create(Task* this, void entrypoint(), bool kernel_task,
                 u32* pagedir) {
    u8* kesp0 = this->stack + 0x1000;
    u8* kesp = kesp0;

    kesp -= sizeof(CPUState);
    this->cpustate = (CPUState*)(kesp);

    memset(this->cpustate, 0, sizeof(CPUState));

    this->cpustate->eip = (u32)entrypoint;
    this->cpustate->eflags = 0x202;

    // Setup selectors

    // NOTE: this value must equal to the offset of the
    // code segment selector in the GDT
    u32 code_selector;
    u32 data_selector;

    if (kernel_task) {
        code_selector = 8;
        data_selector = 16;
    } else {
        code_selector = 24 | 3;
        data_selector = 32 | 3;
    }

    this->cpustate->cs = code_selector;
    this->cpustate->ds = data_selector;
    this->cpustate->es = data_selector;
    this->cpustate->fs = data_selector;
    this->cpustate->gs = data_selector;

    this->cpustate->ss = data_selector;
    this->cpustate->esp = USER_STACK_BOTTOM;

    // Return context
    kesp -= sizeof(TaskReturnContext);
    TaskReturnContext* context = (TaskReturnContext*)kesp;
    context->edi = 0;
    context->esi = 0;
    context->ebx = 0;
    context->ebp = 0;
    context->eip = (u32)isr_exit;

    this->page_dir = pagedir;
    this->kesp0 = (u32)kesp0;
    this->kesp = (u32)kesp;
}

void task_kernel_create(Task* this, void entrypoint()) {
    task_create(this, entrypoint, true, initial_page_dir);
}

void task_user_create(Task* this, void entrypoint()) {
    task_create(this, entrypoint, false, initial_page_dir);
}
