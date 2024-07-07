#include "events.h"
#include "gfx.h"
#include "stdio.h"
#include "stdlib.h"
#include "syscalls.h"

// i like hacky macros :)
#define TEST(test_name, code)               \
    syscall_print("TEST: " test_name "\n"); \
    {code};                                 \
    syscall_print("\n");

// TODO: file system tests
int _start() {
    u32 owner_task_depth = syscall_owner_task_depth();
    // Child task
    if (owner_task_depth > 0) {
        printf("task depth: %i\n", owner_task_depth);
        if (owner_task_depth < 3) {
            syscall_exec_blocking("utest.elf");
        }
        syscall_exit();
    }

    syscall_print("Running tests!\n\n");

    // clang-format off
    TEST("printf", {
        printf("printf: %i %x\n", 123, 0xDEADBEEF);
    });

    TEST("syscall_get_time", {
        printf("systime: %i\n", syscall_get_systime());
    });

    TEST("syscall_owner_task_depth", {
        printf("owner_task_depth: %i\n", syscall_owner_task_depth());
    });

    TEST("heap", {
        printf("heap start: %x\n", syscall_get_heap_start());
        printf("heap end: %x\n", syscall_get_heap_end());
        syscall_set_heap_size(0x4000);
        printf("heap end: %x\n", syscall_get_heap_end());
        syscall_set_heap_size(0);
        printf("heap end: %x\n", syscall_get_heap_end());
    });

    TEST("syscall_sleep", {
        u32 from = syscall_get_systime();
        syscall_sleep(100);
        u32 to = syscall_get_systime();
        u32 delta = to - from;
        printf("delta: %i\n", delta);
    });

    TEST("syscall_scheduler_next", {
        for (u32 i = 0; i < 16; i++) {
            syscall_scheduler_next();
        }
    });

    TEST("task recursion", {
        syscall_exec_blocking("utest.elf");
    });

    TEST("malloc", {
        syscall_set_heap_size(1024 * 128);
        void* heap_start = syscall_get_heap_start();
        add_malloc_block(heap_start, 1024 * 128);

        void* addr = malloc(1024);
        printf("addr: %x\n", addr);
        free(addr);
    });

    TEST("events", {
        EventBuffer* buf = syscall_create_events_buf();
        printf("buf: %x\n", buf);

        printf("Request focus: %i\n", syscall_request_focus());

        events_init(buf);
        printf("Wait for event\n");
        syscall_wait_for_event();

        Event evt = events_pull();
        if (evt.is_keyevent) {
            KeyEvent keyevt = evt.keyevt;
            printf("Key: %c\n", keyevt.c);
        } else {
            MouseEvent mouseevt = evt.mouseevt;
            printf("Mouse: x(%i) y(%i) l(%i) r(%i)\n", mouseevt.delta_x, mouseevt.delta_y, mouseevt.button_left, mouseevt.button_right);
        }
    });

    TEST("rendering", {
        // Screen size
        u32 screen_x = syscall_get_screen_size_x();
        u32 screen_y = syscall_get_screen_size_y();
        printf("Screen X: %i\n", screen_x);
        printf("Screen Y: %i\n", screen_y);

        // Frame buffer creation
        u32* framebuffer = syscall_create_fb(screen_x, screen_y);
        printf("framebuffer: %x\n", framebuffer);

        // Graphics init
        gfx_init((GfxInfo) {
            .width = screen_x,
            .height = screen_y,
            .addr = framebuffer,
        });

        // Simple rendering
        gfx_fill(0xFF000000);
        syscall_draw_fb();

        // Font loading
        printf("loading font\n");

        u32 font_file_id = syscall_file_open("kernelfont.raw");
        printf("font file ID: %i\n", font_file_id);
        u32 font_file_size = syscall_get_file_size(font_file_id);
        printf("font file size: %i\n", font_file_size);
        u32* font_file_buffer = malloc(font_file_size);
        syscall_read(font_file_id, (u8*)font_file_buffer, font_file_size);
        syscall_file_close(font_file_id);

        gfx_load_font(font_file_buffer);

        printf("successfully loaded font\n");

        // Font rendering
        printf("font rendering\n");
        gfx_write(0, 0, "Hello world!", 0xFFFF0000, 0);

        // Render pipeline performance test
        u32 initial_time = syscall_get_systime();

        u32 frames = 0;
        float seconds = 0.0f;
        u32 fps = 0;

        for (u32 i = 0; i < 1000; i++) {
            syscall_draw_fb();
            frames++;
            u32 end_time = syscall_get_systime();
            seconds = ((float)(end_time - initial_time) / 1000.f);
            fps = (u32)(frames / seconds);
        }
        printf("render pipeline average fps: %i\n", fps);

        // Render Test
        for (u32 i = 0; i < screen_x - 100; i++) {
            gfx_fill(0xFF000000);
            gfx_rect(i, 50, 100, 100, 0xFFFF0000);

            char fps_buffer[9];
            snprintf(fps_buffer, sizeof(fps_buffer), "fps: %i", fps);
            gfx_write(0, 0, fps_buffer, 0xFFFF0000, 0);
            syscall_draw_fb();

            frames++;
            u32 end_time = syscall_get_systime();

            seconds = ((float)(end_time - initial_time) / 1000.f);
            fps = (u32)(frames / seconds);
        }
        printf("render test average fps: %i\n", fps);
    });

    // clang-format on

    syscall_exit();
    return 0;
}
