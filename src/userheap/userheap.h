#ifndef USERHEAP_H_
#define USERHEAP_H_

#include "tasks/tasks.h"

void userheap_init(Task* task);
void userheap_set_size(Task* task, u32 size);

#endif
