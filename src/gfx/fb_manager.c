#include "fb_manager.h"

#include "gfx/gfx.h"
#include "memory/shmem.h"
#include "tasks/task_manager.h"
#include "util/debug.h"
#include "util/mem.h"

#define HANDLE_COUNT 16

static FbHandle handles[HANDLE_COUNT];
static i32 current_handle = -1;

void fb_manager_init() {
    memset(handles, 0, sizeof(handles));
    current_handle = 0;
    handles[0].fb_object_addr = (u8*)1;
    handles[0].kernel = true;
    handles[0].task = task_manager_get_task(0);
    handles[0].double_buffering = true;
}

static i32 get_free_handle_slot() {
    for (u32 i = 0; i < HANDLE_COUNT; i++) {
        if (handles[i].fb_object_addr == 0) {
            return i;
        }
    }
    return -1;
}

u32 fb_manager_add(u32 fb_object_id, u8* fb_object_addr, bool double_buffering,
                   Task* task) {
    i32 slot = get_free_handle_slot();
    assert(slot != -1);

    handles[slot].fb_object_id = fb_object_id;
    handles[slot].fb_object_addr = fb_object_addr;
    handles[slot].double_buffering = double_buffering;
    handles[slot].task = task;
    return slot;
}
void fb_manager_remove(u32 handle_id) {
    assert(handle_id < HANDLE_COUNT);
    memset(&handles[handle_id], 0, sizeof(FbHandle));
    if (current_handle == (i32)handle_id) {
        current_handle = -1;
    }
}

u8* fb_manager_map(u32 handle_id) {
    assert(handle_id < HANDLE_COUNT);
    assert(handles[handle_id].fb_object_addr);

    FbHandle handle = handles[handle_id];

    if ((i32)handle_id == current_handle) {
        return handle.fb_object_addr;
    }

    if (current_handle != -1 && !handles[current_handle].kernel) {
        shmem_unmap(handles[current_handle].fb_object_id, 0);
    }
    current_handle = handle_id;

    handle.fb_object_addr = shmem_map(handle.fb_object_id, 0);

    gfx_doublebuffering(handle.double_buffering);
    return handle.fb_object_addr;
}

i32 fb_manager_get_current_handle_id() { return current_handle; }

void fb_manager_pause_all() {
    for (u32 i = 0; i < HANDLE_COUNT; i++) {
        if (handles[i].fb_object_addr == 0) {
            continue;
        }
        handles[i].task->state = TASK_STATE_IDLE;
    }
}
