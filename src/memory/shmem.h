#ifndef SHMEM_H_
#define SHMEM_H_


#include "util/types.h"
#define MAX_SHARED_MEM_OBJS 16
#define MAX_SHARED_MEM_HANDLES 16

typedef i32 TaskHandle;

typedef struct {
    u32 num_pages;
    u32* physical_pages;
    TaskHandle task_handle;
} SharedMemObject;

typedef struct {
    u32 shared_mem_object_index;
    u32 vaddr;
} SharedMemHandle;

typedef struct {
    u32 vaddr_start;
    SharedMemHandle handles[MAX_SHARED_MEM_HANDLES];
    u32 num_handles;
} SharedMemPool;

void shmem_init();

u32 shmem_create(u32 size, TaskHandle task_handle);
void shmem_destroy(u32 slot);

void* shmem_map(u32 object_id, TaskHandle task_handle);
void shmem_unmap(u32 id, TaskHandle task_handle);

void shmem_destroy_owned_by(TaskHandle task_handle);

void* shmem_get_vaddr(u32 object_id, TaskHandle task_handle);

#endif
