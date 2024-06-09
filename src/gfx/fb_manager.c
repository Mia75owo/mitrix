#include "fb_manager.h"

#include "gfx/gfx.h"
#include "memory/shmem.h"
#include "util/debug.h"
#include "util/mem.h"

#define HANDLE_COUNT 16

static FbHandle handles[HANDLE_COUNT];
static i32 current_handle = -1;

void fb_manager_init() {
    memset(handles, 0, sizeof(handles));
    current_handle = -1;
}

static i32 get_free_handle_slot() {
    for (u32 i = 0; i < HANDLE_COUNT; i++) {
        if (handles[i].fb_object_addr == 0) {
            return i;
        }
    }
    return -1;
}

u32 fb_manager_add(u32 fb_object_id, u8* fb_object_addr,
                   bool double_buffering) {
    i32 slot = get_free_handle_slot();
    assert(slot != -1);

    handles[slot].fb_object_id = fb_object_id;
    handles[slot].fb_object_addr = fb_object_addr;
    handles[slot].double_buffering = double_buffering;
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

    if (current_handle != -1) {
        shmem_unmap(handles[current_handle].fb_object_id, 0);
    }
    current_handle = handle_id;

    handle.fb_object_addr = shmem_map(handle.fb_object_id, 0);

    if (!handle.double_buffering) {
        gfx_doublebuffering(false);
    }
    return handle.fb_object_addr;
}

i32 fb_manager_get_current_handle_id() { return current_handle; }
