#include "stdio.h"
#include "syscalls.h"
#include "events.h"
#include "gfx.h"
#include "stdlib.h"

int _start() {
    u32* addr = syscall_create_fb();
    printf("fb_addr: %x\n", addr);
    GfxInfo gfx_info;
    gfx_info.width = 800;
    gfx_info.height = 600;
    gfx_info.bpp = 8;
    gfx_info.pitch = 0;
    gfx_info.addr = addr;
    gfx_init(gfx_info);

    EventBuffer* events_buf = syscall_create_events_buf();
    printf("events_buf: %x\n", events_buf);

    events_init(events_buf);

    syscall_request_screen();

    static i32 pos = 0;

    gfx_fill(0xFFFF0000);
    gfx_rect(pos, 0, 100, 100, 0xFF00FF00);
    syscall_draw_fb();

    while (true) {
        if (!events_has_event()) continue;

        KeyEvent evt = events_pull();
        // Ignore keyup
        if (!evt.pressed) continue;

        if (evt.special && evt.c == 101) {
            pos -= 10;
        } else if (evt.special && evt.c == 103) {
            pos += 10;
        } else if (evt.c == 'q') {
            break;
        }

        gfx_fill(0xFFFF0000);
        gfx_rect(pos, 0, 100, 100, 0xFF00FF00);
    
        syscall_draw_fb();
    }

    syscall_set_heap_size(0x1000 * 2);
    printf("start: %x\n", syscall_get_heap_start());
    printf("end: %x\n", syscall_get_heap_end());

    void* heap_start = syscall_get_heap_start();
    add_malloc_block(heap_start, 0x1000 * 2);

    void* a = malloc(64);
    void* b = malloc(64);
    void* c = malloc(64);

    printf("%x\n", a);
    printf("%x\n", b);
    printf("%x\n", c);

    free(a);
    free(b);
    free(c);

    syscall_exit();

    return 0;
}
