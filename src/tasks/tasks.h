#ifndef TASKS_H_
#define TASKS_H_

#include "util/types.h"
#include "idt/idt.h"

#define TASKS_COUNT 16

typedef struct {
    u8 stack[0x1000];
    CPUState* cpustate;
} Task;


void task_create(Task* this, void entrypoint());
void tasks_init();
void tasks_add_task(Task* task);
CPUState* tasks_scedule(CPUState* cpustate);

#endif
