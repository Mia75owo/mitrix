#ifndef TASKMGR_H_
#define TASKMGR_H_

#include "tasks/tasks.h"
#include "util/types.h"

#define NUM_TASKS 32

typedef i32 TaskHandle;

void traskmgr_init();

TaskHandle taskmgr_create_kernel_task(void entrypoint());
TaskHandle taskmgr_create_user_task(char* elf_file_name);

Task* taskmgr_handle_to_pointer(TaskHandle handle);
TaskHandle taskmgr_handle_from_pointer(Task* ptr);

TaskHandle taskmgr_get_current_task();

void taskmgr_kill_task(TaskHandle handle);
void taskmgr_set_state(TaskHandle handle, TaskState state);
void taskmgr_focus_task(TaskHandle handle);

void taskmgr_schedule();

#endif
