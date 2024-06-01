#include "shmem.h"

#include "memory/kmalloc.h"
#include "memory/memory.h"
#include "memory/pmm.h"
#include "tasks/task_manager.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/sys.h"

static SharedMemObject shared_mem_objects[MAX_SHARED_MEM_OBJS];
static SharedMemPool kernel_shpool;

void shmem_init() {
    memset(&shared_mem_objects, 0, sizeof(shared_mem_objects));
    memset(&kernel_shpool, 0, sizeof(kernel_shpool));
    kernel_shpool.vaddr_start = KERNEL_SHARED_MEM;
}

static i32 find_available_obj_slot() {
    for (u32 i = 0; i < MAX_SHARED_MEM_OBJS; i++) {
        if (shared_mem_objects[i].num_pages == 0) {
            return i;
        }
    }
    return -1;
}

u32 shmem_create(u32 size, u32 owner_task_index) {
    i32 slot = find_available_obj_slot();
    assert(slot >= 0);

    SharedMemObject* obj = &shared_mem_objects[slot];

    obj->owner_task_index = owner_task_index;

    u32 pages = CEIL_DIV(size, 0x1000);
    obj->num_pages = pages;
    obj->physical_pages = (u32*)kmalloc(pages * sizeof(u32));

    for (u32 i = 0; i < pages; i++) {
        obj->physical_pages[i] = pmm_alloc_pageframe();
    }
    return slot;
}

void shmem_destroy(u32 slot) {
    SharedMemObject* obj = &shared_mem_objects[slot];

    for (u32 i = 0; i < obj->num_pages; i++) {
        pmm_free_pageframe(obj->physical_pages[i]);
    }
    kfree((void*)obj->physical_pages);
    memset(obj, 0, sizeof(SharedMemObject));
}

static SharedMemHandle* insert_into_pool(SharedMemPool* pool, u32 object_id) {
    assert(pool);
    assert(pool->num_handles < MAX_SHARED_MEM_HANDLES);

    SharedMemHandle* handle = &pool->handles[pool->num_handles];
    handle->shared_mem_object_index = object_id;
    handle->vaddr = pool->vaddr_start;

    if (pool->num_handles > 0) {
        SharedMemHandle* prev = &pool->handles[pool->num_handles - 1];
        u32 prev_size =
            shared_mem_objects[prev->shared_mem_object_index].num_pages;
        handle->vaddr = prev->vaddr + prev_size * 0x1000;
    }
    pool->num_handles++;
    return handle;
}

static void remove_from_pool(SharedMemPool* pool, u32 handle_index) {
    assert(handle_index < pool->num_handles);

    // Shift entries back
    for (u32 i = handle_index; i < pool->num_handles - 1; i++) {
        pool->handles[i] = pool->handles[i + 1];
    }

    pool->num_handles--;
}

void* shmem_map(u32 object_id, u32 task_id) {
    cli_push();

    SharedMemObject* obj = &shared_mem_objects[object_id];

    SharedMemPool* pool;
    if (task_id == 0) {
        pool = &kernel_shpool;
    } else {
        Task* task = task_manager_get_task(task_id);
        pool = &task->shmem_pool;
    }

    SharedMemHandle* handle = insert_into_pool(pool, task_id);
    assert(handle);

    bool map_to_kernel = task_id == 0;
    u32* prev_pd = NULL;
    if (!map_to_kernel) {
        prev_pd = memory_get_current_page_dir();
        u32* new_pd = task_manager_get_task(task_id)->page_dir;
        memory_change_page_dir(new_pd);
    }

    for (u32 i = 0; i < obj->num_pages; i++) {
        u32 flags = PTE_READ_WRITE;
        if (!map_to_kernel) flags |= PTE_USER;

        memory_map_page(handle->vaddr + i * 0x1000, obj->physical_pages[i],
                        flags);
    }

    if (!map_to_kernel) {
        memory_change_page_dir(prev_pd);
    }

    assert(handle->vaddr);
    cli_pop();
    return (void*)handle->vaddr;
}

void shmem_unmap(u32 id, u32 task_id) {
    SharedMemObject* obj = &shared_mem_objects[id];

    SharedMemPool* pool;
    if (task_id == 0) {
        pool = &kernel_shpool;
    } else {
        Task* task = task_manager_get_task(task_id);
        pool = &task->shmem_pool;
    }

    assert(pool);

    SharedMemHandle* handle = NULL;
    i32 pool_index = -1;
    for (u32 i = 0; i < MAX_SHARED_MEM_HANDLES; i++) {
        if (pool->handles[i].shared_mem_object_index == id) {
            handle = &pool->handles[i];
            break;
        }
    }

    assert(handle);

    bool unmap_from_kernel = task_id == 0;
    u32* prev_pd = NULL;
    if (!unmap_from_kernel) {
        prev_pd = memory_get_current_page_dir();
        u32* new_pd = task_manager_get_task(task_id)->page_dir;
        memory_change_page_dir(new_pd);
    }

    for (u32 i = 0; i < obj->num_pages; i++) {
        memory_unmap_page(handle->vaddr + i * 0x1000);
    }

    if (!unmap_from_kernel) {
        memory_change_page_dir(prev_pd);
    }

    remove_from_pool(pool, pool_index);
}

void shmem_destroy_owned_by(u32 task_id) {
    for (u32 i = 0; i < MAX_SHARED_MEM_OBJS; i++) {
        if (shared_mem_objects[i].owner_task_index == task_id) {
            shmem_destroy(i);
        }
    }
}
