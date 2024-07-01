#ifndef FB_MANAGER_H
#define FB_MANAGER_H

#include "tasks/tasks.h"
#include "util/types.h"

typedef struct {
    u32 fb_object_id;
    u8* fb_object_addr;
    bool double_buffering;

    Task* task;
    bool kernel;
} FbHandle;

void fb_manager_init();

u32 fb_manager_add(u32 fb_object_id, u8* fb_object_addr, bool double_buffering, Task* task);
void fb_manager_remove(u32 handle_id);

u8* fb_manager_map(u32 handle_id);

i32 fb_manager_get_current_handle_id();
void fb_manager_pause_all();

#endif