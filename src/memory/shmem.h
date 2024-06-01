#ifndef SHMEM_H_
#define SHMEM_H_

#include "util/types.h"
#define MAX_SHARED_MEM_OBJS 16
#define MAX_SHARED_MEM_HANDLES 16

typedef struct {
    u32 num_pages;
    u32* physical_pages;
    u32 owner_task_index;
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

u32 shmem_create(u32 size, u32 owner_task_index);
void shmem_destroy(u32 slot);

void* shmem_map(u32 object_id, u32 task_id);
void shmem_unmap(u32 id, u32 task_id);

void shmem_destroy_owned_by(u32 task_id);

#endif
