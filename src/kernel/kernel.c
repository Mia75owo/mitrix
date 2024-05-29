#include "disk/disk.h"
#include "fpu/fpu.h"
#include "gdt/gdt.h"
#include "gfx/gfx.h"
#include "gfx/gui.h"
#include "gfx/tty.h"
#include "keyboard/keyboard.h"
#include "memory/kmalloc.h"
#include "memory/memory.h"
#include "multiboot.h"
#include "pit/pit.h"
#include "serial/serial.h"
#include "tasks/tasks.h"
#include "tests/tests.h"
#include "util/debug.h"
#include "util/mem.h"
#include "util/sys.h"
#include "util/types.h"

extern void gdt_load();
extern void idt_load();

#define kinit(call, name)                            \
    klog("%0F[ %0BINFO %0F] %07Loading %s\n", name); \
    (call);                                          \
    klog("%0F[  %0AOK  %0F] %07%s Success\n", name);

void gui_loop();

void kernel_main(u32 magic, struct multiboot_info* boot_info) {
    gui_init_early_tty();
    tty_reset();

    assert_msg(magic == 0x2badb002, "Kernel magic parameter wrong!");

    kinit(gdt_init(), "GDT");
    kinit(idt_load(), "IDT");
    kinit(serial_init(), "Serial");
    kinit(fpu_init(), "FPU");
    kinit(keyboard_init(), "Keyboard");
    kinit(pit_init(1000), "PIT");
    kinit(tasks_init(), "Tasks");
    kinit(disk_init(boot_info), "Ramdisk");

    gfx_info gfx_data = {
        .width = boot_info->framebuffer_width,
        .height = boot_info->framebuffer_height,
        .bpp = boot_info->framebuffer_bpp,
        .pitch = boot_info->framebuffer_pitch,
        .addr = (u32*)(u32)boot_info->framebuffer_addr,
    };

    {
        u32 mem_high = boot_info->mem_upper * 1024;
        u32 physical_alloc_start = 0x100000 * 16;
        kinit(memory_init(mem_high, physical_alloc_start), "Memory");
    }

    kinit(kmalloc_init(), "Kmalloc");

    disk_map();
    disk_init_fs();

    kinit(gfx_init(gfx_data), "GFX");
    gfx_fill(0x111111);
    // gfx_debug(GFX_DEBUG_FONT_FILL);

    /*debug_tests();*/

    klog("\n");
    klog("%00 %10 %20 %30 %40 %50 %60 %70 %80 %90 %A0 %B0 %C0 %D0 %E0 %F0 \n");
    klog("hi :D");

    memory_set_boot_info(boot_info);

    gui_init(gfx_data.width, gfx_data.height);

    Task gui_task;
    task_kernel_create(&gui_task, gui_loop);
    tasks_add_task(&gui_task);

    Task user_task;
    task_user_create(&user_task, "user.exe");
    tasks_add_task(&user_task);

    asm volatile("sti");
    spin_halt();
}

void gui_loop() {
    u64 last_draw = 0;
    while (true) {
        u64 tics = pit_get_tics();

        if (tics - last_draw > 100) {
            gui_redraw();
        }
    }
}
