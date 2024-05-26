#ifndef TASKS_H_
#define TASKS_H_

#include "idt/idt.h"
#include "util/types.h"

#define TASKS_COUNT 16

typedef struct {
    u32 kesp;   // esp
    u32 kesp0;  // esp top (stack + sizeof(stack))
    u32* page_dir;

    CPUState* cpustate;

    u8 stack[0x1000];
} Task;

typedef struct {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    u32 eip;
} TaskReturnContext;

void task_create(Task* this, void entrypoint(), bool kernel_task, u32* pagedir);
void task_kernel_create(Task* this, void entrypoint());
void task_user_create(Task* this, void entrypoint());

void tasks_init();
void tasks_add_task(Task* task);
void tasks_schedule();

#endif
