#include "kmalloc.h"

#include "gfx/vtty.h"
#include "memory/memory.h"
#include "memory/pmm.h"
#include "util/debug.h"
#include "util/mem.h"

static void print_buddy(u32* buddy, u32 num_bits, u32 max_bits);
static u32 big_allocation_size(u32 chunk);
static void big_allocation_remove(u32 chunk);
static void big_allocation_add(u32 chunk, u32 length);

#define EXACT_ALLOC_SIZE (1024 * 1024 * 64)

#define BUDDY64_RAW_SIZE (EXACT_ALLOC_SIZE / 64 / 32)
#define BUDDY512_RAW_SIZE (EXACT_ALLOC_SIZE / 512 / 32)
#define BUDDY4096_RAW_SIZE (EXACT_ALLOC_SIZE / 4096 / 32)

static u32 buddy64[BUDDY64_RAW_SIZE];
static u32 buddy512[BUDDY512_RAW_SIZE];
static u32 buddy4096[BUDDY4096_RAW_SIZE];

static u32 allocated_by_user512[BUDDY512_RAW_SIZE];
static u32 allocated_by_user4096[BUDDY512_RAW_SIZE];

#define BIG_ALLOCATIONS_SIZE 64
static struct {
    u32 start;
    u32 length;
} big_allocations[BIG_ALLOCATIONS_SIZE];

static bool initialized = false;

void kmalloc_init() {
    memset(buddy64, 0, sizeof(buddy64));
    memset(buddy512, 0, sizeof(buddy512));
    memset(buddy4096, 0, sizeof(buddy4096));

    memset(big_allocations, 0, sizeof(big_allocations));

    // Map pages
    u32 pages = EXACT_ALLOC_SIZE / 0x1000;
    for (u32 i = 0; i < pages; i++) {
        u32 phys_addr = pmm_alloc_pageframe();
        memory_map_page(KERNEL_MALLOC + i * 0x1000, phys_addr, PTE_READ_WRITE);
    }

    initialized = true;
}

u8* kmalloc(u32 size) {
    assert_msg(initialized, "KMALLOC: use before initialization");
    assert_msg(size, "KMALLOC: (size == 0)");

    if (size <= 64) {
        i32 found_index = 0;

        for (u32 i = 0; i < BUDDY64_RAW_SIZE; i++) {
            if (buddy64[i] == 0xFFFFFFFF) continue;

            for (u32 j = 0; j < 32; j++) {
                if (BITMAP_GET(buddy64, i * 32 + j) == 0) {
                    found_index = i * 32 + j;
                    goto found_64;
                }
            }
        }

        assert_msg(0, "KMALLOC: no place for 64 bit chunk");
    found_64:
        // Update buddy bitmaps
        BITMAP_SET(buddy64, found_index, 1);
        BITMAP_SET(buddy512, found_index / 8, 1);
        BITMAP_SET(buddy4096, found_index / 64, 1);

        return (u8*)(KERNEL_MALLOC + found_index * 64);
    } else if (size <= 512) {
        i32 found_index = 0;

        for (u32 i = 0; i < BUDDY512_RAW_SIZE; i++) {
            if (buddy512[i] == 0xFFFFFFFF) continue;

            for (u32 j = 0; j < 32; j++) {
                if (BITMAP_GET(buddy512, i * 32 + j) == 0) {
                    found_index = i * 32 + j;
                    goto found_512;
                }
            }
        }

        assert_msg(0, "KMALLOC: no place for 512 bit chunk");
    found_512:
        // Update buddy bitmaps
        for (u32 i = 0; i < 8; i++) {
            BITMAP_SET(buddy64, found_index * 8 + i, 1);
        }
        BITMAP_SET(buddy512, found_index, 1);
        BITMAP_SET(buddy4096, found_index / 8, 1);

        BITMAP_SET(allocated_by_user512, found_index, 1);

        return (u8*)(KERNEL_MALLOC + found_index * 512);
    } else if (size <= 4096) {
        i32 found_index = 0;

        for (u32 i = 0; i < BUDDY4096_RAW_SIZE; i++) {
            if (buddy4096[i] == 0xFFFFFFFF) continue;

            for (u32 j = 0; j < 32; j++) {
                if (BITMAP_GET(buddy4096, i * 32 + j) == 0) {
                    found_index = i * 32 + j;
                    goto found_4096;
                }
            }
        }

        assert_msg(0, "KMALLOC: no place for 4096 bit chunk");
    found_4096:
        // Update buddy bitmaps
        for (u32 i = 0; i < 64; i++) {
            BITMAP_SET(buddy64, found_index * 64 + i, 1);
        }
        for (u32 i = 0; i < 8; i++) {
            BITMAP_SET(buddy512, found_index * 8 + i, 1);
        }
        BITMAP_SET(buddy4096, found_index, 1);

        BITMAP_SET(allocated_by_user4096, found_index, 1);

        return (u8*)(KERNEL_MALLOC + found_index * 4096);
    } else {
        u32 chunk_count = size / 4096;

        u32 found_length = 0;
        i32 found_index = -1;
        for (u32 i = 0; i < BUDDY4096_RAW_SIZE * 32; i++) {
            if (BITMAP_GET(buddy4096, i) == 0) {
                found_length++;
                if (found_length >= chunk_count) {
                    found_index = i - (found_length - 1);
                    break;
                }
            } else {
                found_length = 0;
            }
        }

        assert_msg(found_index != -1, "KMALLOC: no place for big allocation");

        for (u32 chunk = 0; chunk < chunk_count; chunk++) {
            // Update buddy bitmaps
            for (u32 i = 0; i < 64; i++) {
                BITMAP_SET(buddy64, (found_index + chunk) * 64 + i, 1);
            }
            for (u32 i = 0; i < 8; i++) {
                BITMAP_SET(buddy512, (found_index + chunk) * 8 + i, 1);
            }
            BITMAP_SET(buddy4096, (found_index + chunk), 1);
            BITMAP_SET(allocated_by_user4096, (found_index + chunk), 1);
        }

        big_allocation_add(found_index, chunk_count);
        return (u8*)(KERNEL_MALLOC + found_index * 4096);
    }
}

void kfree(u8* addr) {
    assert_msg(addr >= (u8*)KERNEL_MALLOC, "KFREE: invalid address (to low)");
    assert_msg(addr <= (u8*)(KERNEL_MALLOC + EXACT_ALLOC_SIZE),
               "KFREE: invalid address (to high)");
    assert_msg((u32)addr % 64 == 0, "KFREE: invalid address (!64bit aligned)");

    u32 relative_addr = (u32)addr - KERNEL_MALLOC;
    assert_msg(BITMAP_GET(buddy64, relative_addr / 64), "KFREE: double free");

    // Was allocated as a 4096 chunk?
    if (BITMAP_GET(allocated_by_user4096, relative_addr / 4096)) {
        // Get chunk count (could be multiple 4096 chunks)
        u32 chunk_count = big_allocation_size(relative_addr / 4096);
        for (u32 chunk = 0; chunk < big_allocation_size(relative_addr / 4096);
             chunk++) {
            u32 current_addr = (relative_addr / 4096 + chunk) * 4096;

            BITMAP_SET(allocated_by_user4096, current_addr / 4096, 0);

            // Mark 4096 as free
            BITMAP_SET(buddy4096, current_addr / 4096, 0);
            // Mark 512 as free
            for (u32 i = 0; i < (4096 / 512); i++) {
                BITMAP_SET(buddy512, (current_addr / 512) + i, 0);
            }
            // Mark 64 as free
            for (u32 i = 0; i < (4096 / 64); i++) {
                BITMAP_SET(buddy64, (current_addr / 64) + i, 0);
            }
        }
        // Remove from big allocation index
        if (chunk_count != 1) {
            big_allocation_remove(relative_addr / 4096);
        }
        return;
    }

    // Was allocated as a 512 chunk?
    if (BITMAP_GET(allocated_by_user512, relative_addr / 512)) {
        BITMAP_SET(allocated_by_user512, relative_addr / 512, 0);

        // Mark 512 as free
        BITMAP_SET(buddy512, relative_addr / 512, 0);
        // Mark 64 as free
        for (u32 i = 0; i < (512 / 64); i++) {
            BITMAP_SET(buddy64, (relative_addr / 64) + i, 0);
        }
        goto check_4096_free;
    }

    // Was allocated as a 64 chunk
    BITMAP_SET(buddy64, relative_addr / 64, 0);

    // Check if 512 is free
    for (u32 i = 0; i < (512 / 64); i++) {
        if (BITMAP_GET(buddy64, (relative_addr / 512) * (512 / 64) + i)) {
            return;
        }
    }
    // Mark as free
    BITMAP_SET(buddy512, relative_addr / 512, 0);

check_4096_free:
    for (u32 i = 0; i < (4096 / 512); i++) {
        if (BITMAP_GET(buddy512, (relative_addr / 4096) * (4096 / 512) + i)) {
            return;
        }
    }
    // Mark as free
    BITMAP_SET(buddy4096, relative_addr / 4096, 0);
    return;
}

static void print_buddy(u32* buddy, u32 num_bits, u32 max_bits) {
    assert_msg(initialized, "KMALLOC: use before initialization");

    for (u32 i = 0; i < num_bits; i++) {
        if (i <= max_bits - 4) {
            if (BITMAP_GET(buddy, i)) {
                klog("%C0 ");  // used
            } else {
                klog("%A0 ");  // unused
            }
        } else if (i < max_bits) {
            if (BITMAP_GET(buddy, i)) {
                klog("%0C.");  // used
            } else {
                klog("%0A.");  // unused
            }
        } else {
            break;
        }
    }
}

static u32 big_allocation_size(u32 chunk) {
    for (u32 i = 0; i < BIG_ALLOCATIONS_SIZE; i++) {
        if (big_allocations[i].start == chunk &&
            big_allocations[i].length != 0) {
            return big_allocations[i].length;
        }
    }
    return 1;
}
static void big_allocation_add(u32 chunk, u32 length) {
    for (u32 i = 0; i < BIG_ALLOCATIONS_SIZE; i++) {
        if (big_allocations[i].length == 0) {
            big_allocations[i].start = chunk;
            big_allocations[i].length = length;
            return;
        }
    }
    assert_msg(0, "KMALLOC: no place in BIG_ALLOCATION index");
}
static void big_allocation_remove(u32 chunk) {
    for (u32 i = 0; i < BIG_ALLOCATIONS_SIZE; i++) {
        if (big_allocations[i].start == chunk &&
            big_allocations[i].length != 0) {
            big_allocations[i].start = 0;
            big_allocations[i].length = 0;
            return;
        }
    }
}

void kmalloc_print_info() {
    klog("\n\n%40KMALLOC_INFO:\n%03%[@|=]");

    klog("%0F4096: \n");
    print_buddy(buddy4096, BUDDY4096_RAW_SIZE * 32, VTTY_WIDTH * 4);
    klog("\n");
    klog("%0F512: \n");
    print_buddy(buddy512, BUDDY512_RAW_SIZE * 32, VTTY_WIDTH * 4);
    klog("%0F64: \n");
    print_buddy(buddy64, BUDDY64_RAW_SIZE * 32, VTTY_WIDTH * 4);

    klog("%[@|=]%40KMALLOC_INFO_END\n");
}
