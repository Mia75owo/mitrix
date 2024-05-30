#ifndef TASK_MANAGER_H_
#define TASK_MANAGER_H_

#include "tasks.h"

#define TASKS_COUNT 16

void task_manager_init();
void task_manager_schedule();

void task_manager_kill_current_task();
bool task_manager_current_task_alive();

Task* create_kernel_task(void entrypoint());
Task* create_user_task(char* elf_file_name);


#endif
