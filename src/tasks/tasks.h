#ifndef TASKS_H_
#define TASKS_H_

#include "idt/idt.h"
#include "memory/shmem.h"
#include "util/types.h"

#define TASK_STACK_SIZE 0x1000

typedef enum {
    TASK_STATE_DEAD,
    TASK_STATE_IDLE,
    TASK_STATE_RUNNING,
} TaskState;

typedef struct {
    u32 kesp;   // esp
    u32 kesp0;  // esp top
    u32* page_dir;

    CPUState* cpustate;

    TaskState state;
    u8* raw_elf;

    SharedMemPool sh_mem_pool;

    u8* stack;
} Task;

typedef struct {
    u32 edi;
    u32 esi;
    u32 ebx;
    u32 ebp;
    u32 eip;
} TaskReturnContext;

void task_kernel_create(Task* this, void entrypoint());
void task_user_create(Task* this, char* elf_file_name);
void task_kill(Task* task);

void task_switch(Task* old, Task* new);

#endif
