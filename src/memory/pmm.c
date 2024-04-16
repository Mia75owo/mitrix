#include "pmm.h"

#include "util/debug.h"
#include "util/mem.h"

#define NUM_PAGE_FRAMES (0x100000000 / 0x1000 / 8)
#define PAGE_FRAME_SIZE 0x1000

static u32 page_frame_min;
static u32 page_frame_max;
static u32 total_alloc;

u8 physical_memory_bitmap[NUM_PAGE_FRAMES / 8];

static bool is_pf_used(u32 pf_index) {
    u8 byte = physical_memory_bitmap[pf_index / 8];
    return byte >> (pf_index & 7) & 1;
}
static void set_pf_used(u32 pf_index, bool used) {
    u8 byte = physical_memory_bitmap[pf_index / 8];
    byte ^= (-used ^ byte) & (1 << (pf_index & 7));

    physical_memory_bitmap[pf_index / 8] = byte;
}

void pmm_init(u32 mem_low, u32 mem_high) {
    page_frame_min = CEIL_DIV(mem_low, 0x1000);
    page_frame_max = mem_high / 0x1000;

    klog("low: %x, high: %x\n", (u64)mem_low, (u64)mem_high);

    total_alloc = 0;
    memset(physical_memory_bitmap, 0, sizeof(physical_memory_bitmap));
}

u32 pmm_alloc_pageframe() {
    u32 start = page_frame_min / 8 + ((page_frame_min & 7) != 0 ? 1 : 0);
    u32 end = page_frame_max / 8 - ((page_frame_max & 7) != 0 ? 1 : 0);

    for (u32 b = start; b < end; b++) {
        u8 byte = physical_memory_bitmap[b];
        if (byte == 0xFF) continue;

        for (u32 i = 0; i < 8; i++) {
            // extract from bitmap
            bool used = byte >> i & 1;

            if (!used) {
                byte ^= (-1 ^ byte) & (1 << i);
                physical_memory_bitmap[b] = byte;
                total_alloc++;
                u32 addr = (b * 8 + i) * PAGE_FRAME_SIZE;
                return addr;
            }
        }
    }

    assert_msg(false, "OUT OF MEMORY!");
    return 0;
}

void pmm_free_pageframe(u32 addr) {
    u32 pf_index = addr / PAGE_FRAME_SIZE;

    assert_msg(is_pf_used(pf_index),
               "Can not free; Page is already not in use!");

    set_pf_used(pf_index, false);
    total_alloc--;
}

u32 pmm_get_total_allocated_pages() { return total_alloc; }
