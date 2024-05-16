#include "tasks.h"

#include "memory/memory.h"
#include "tasks/tss.h"
#include "util/debug.h"
#include "util/mem.h"

static Task* tasks[TASKS_COUNT];
static u32 num_tasks;
static i32 current_task;

void tasks_init() {
    num_tasks = 0;
    current_task = -1;
}

void tasks_add_task(Task* task) {
    assert_msg(num_tasks <= TASKS_COUNT, "MAX TASK SIZE EXEEDED");
    tasks[num_tasks++] = task;
}

CPUState* tasks_scedule(CPUState* cpustate) {
    if (num_tasks <= 0) {
        return cpustate;
    }

    if (current_task >= 0) {
        tasks[current_task]->cpustate = cpustate;
    }

    if (++current_task >= (i32)num_tasks) {
        current_task %= num_tasks;
    }

    if (tasks[current_task]->cpustate->cs == 24) {
        asm volatile("cli");
    }
    return tasks[current_task]->cpustate;
}

void task_create(Task* this, void entrypoint()) {
    this->cpustate = (CPUState*)(this->stack + 0x1000 - sizeof(CPUState));

    memset(this->cpustate, 0, sizeof(CPUState));

    this->cpustate->eip = (u32)entrypoint;
    this->cpustate->eflags = 0x202;

    // NOTE: this value must equal to the offset of the
    // code segment selector in the GDT
    this->cpustate->cs = 0x8;
}
