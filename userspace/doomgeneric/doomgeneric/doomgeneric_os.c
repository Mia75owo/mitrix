#include "doomgeneric.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "syscalls.h"
#include "events.h"

#undef FEATURE_DEHACKED

#define HEAP_SIZE (0x1000 * 256 * 10)

u32* frame_buffer = NULL;
EventBuffer* event_buffer = NULL;

int _start() {
    syscall_set_heap_size(HEAP_SIZE);
    void* heap_start = syscall_get_heap_start();
    add_malloc_block(heap_start, HEAP_SIZE);

    char* argv[] = {"doom", "-iwad", "doom1.wad"};

    printf("DOOM create\n");
    doomgeneric_Create(3, (char**)argv);
    printf("DOOM create done\n");

    while(true) {
        printf("DOOM tick\n");
        doomgeneric_Tick();
    }

    syscall_exit();
    return 0;
}

void DG_Init() {
    frame_buffer = syscall_create_fb();
    event_buffer = syscall_create_events_buf();

    events_init(event_buffer);
    syscall_request_screen();
}

void DG_DrawFrame() {
    DG_ScreenBuffer = frame_buffer;
    syscall_draw_fb();
}
void DG_SleepMs(uint32_t ms) {}
uint32_t DG_GetTicksMs() { return syscall_get_systime(); }
int DG_GetKey(int* pressed, unsigned char* doomKey) {}
void DG_SetWindowTitle(const char* tile) {}
