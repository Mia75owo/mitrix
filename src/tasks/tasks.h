#ifndef TASKS_H_
#define TASKS_H_

#include "disk/mifs.h"
#include "idt/idt.h"
#include "memory/shmem.h"
#include "util/types.h"

#define TASK_STACK_SIZE 0x1000
#define TASK_MAX_FILES 16

typedef enum {
    TASK_STATE_DEAD,
    TASK_STATE_IDLE,
    TASK_STATE_RUNNING,
    TASK_STATE_SLEEPING,
    TASK_STATE_WAIT_FOR_EVENT,
    TASK_STATE_BLOCKED_BY_EXEC,
} TaskState;

typedef struct {
    FilePtr file;
    u32 offset;
} TaskOpenFile;

typedef struct {
    u32 kesp;   // esp
    u32 kesp0;  // esp top
    u32* page_dir;

    CPUState* cpustate;

    TaskState state;
    u8* raw_elf;

    SharedMemPool shmem_pool;
    i32 fb_handle_id;
    i32 shmem_events_obj;

    TaskOpenFile files[TASK_MAX_FILES];
    u32 heap_start;
    u32 heap_end;

    u32 sleep_timestamp;
    bool pending_events;

    TaskHandle owner_task;

    bool is_kernel_task;

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
