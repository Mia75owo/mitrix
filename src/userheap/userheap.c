#include "userheap.h"

#include "memory/kmalloc.h"
#include "memory/memory.h"
#include "memory/pmm.h"
#include "util/mem.h"

void userheap_init(Task* task) {
    task->heap_start = USER_HEAP_START;
    task->heap_end = USER_HEAP_START;
}
void userheap_set_size(Task* task, u32 size) {
    u32 old_size = task->heap_end - task->heap_start;
    u32 old_num_pages = CEIL_DIV(old_size, 0x1000);

    u32 num_pages = CEIL_DIV(size, 0x1000);

    if (old_num_pages == num_pages) {
        return;
    }

    u32* prev_pd = memory_get_current_page_dir();
    memory_change_page_dir(task->page_dir);

    // Free pages if size increased
    if (old_num_pages > num_pages) {
        for (u32 i = num_pages; i < old_num_pages; i++) {
            pmm_free_pageframe(task->physical_heap_pages[i]);
            memory_unmap_page(task->heap_start + i * 0x1000);
        }

        // Update the pages array
        void* new_physical_pages_array = kmalloc(num_pages * sizeof(u32));
        if (task->physical_heap_pages != 0) {
            memcpy(new_physical_pages_array, task->physical_heap_pages,
                   num_pages * sizeof(u32));
            kfree((void*)task->physical_heap_pages);
        }
        task->physical_heap_pages = new_physical_pages_array;
    } else {
        // Update the pages array
        void* new_physical_pages_array = kmalloc(num_pages * sizeof(u32));
        if (task->physical_heap_pages != 0) {
            memcpy(new_physical_pages_array, task->physical_heap_pages,
                   old_num_pages * sizeof(u32));
            kfree((void*)task->physical_heap_pages);
        }
        task->physical_heap_pages = new_physical_pages_array;

        for (u32 i = old_num_pages; i < num_pages; i++) {
            u32 phys_addr = pmm_alloc_pageframe();
            task->physical_heap_pages[i] = phys_addr;

            u32 flags = PTE_OWNER | PTE_READ_WRITE | PTE_USER;
            memory_map_page(task->heap_start + i * 0x1000, phys_addr, flags);
        }
    }

    task->heap_end = task->heap_start + num_pages * 0x1000;

    memory_change_page_dir(prev_pd);
}
