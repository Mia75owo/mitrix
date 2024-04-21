#include "fpu/fpu.h"
#include "gfx/gfx.h"
#include "gfx/vtty.h"
#include "keyboard/keyboard.h"
#include "memory/memory.h"
#include "multiboot.h"
#include "pit/pit.h"
#include "serial/serial.h"
#include "tests/tests.h"
#include "tty/tty.h"
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

void kernel_main(u32 magic, struct multiboot_info* boot_info) {
    assert_msg(magic == 0x2badb002, "Kernel magic parameter wrong!");
    (void)boot_info;

    bool is_graphical =
        boot_info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB;

    if (is_graphical) {
        vtty_init();
    }
    tty_reset();

    kinit(gdt_load(), "GDT");
    kinit(idt_load(), "IDT");
    kinit(serial_init(), "Serial");
    kinit(fpu_init(), "FPU");
    kinit(keyboard_init(), "Keyboard");
    kinit(pit_init(1000), "PIT");

    gfx_info gfx_data = {
        .width = boot_info->framebuffer_width,
        .height = boot_info->framebuffer_height,
        .bpp = boot_info->framebuffer_bpp,
        .pitch = boot_info->framebuffer_pitch,
        .addr = (u32*)boot_info->framebuffer_addr,
    };

    {
        u32 mem_high = boot_info->mem_upper * 1024;
        u32 physical_alloc_start = 0x100000 * 16;
        kinit(memory_init(mem_high, physical_alloc_start), "Memory");
    }

    if (is_graphical) {
        kinit(gfx_init(gfx_data), "GFX");
        gfx_fill(0x111111);

        vtty_set_ready(true);
        // gfx_debug(GFX_DEBUG_FONT_FILL);
    }


    asm volatile("sti");

    /*debug_tests();*/

    klog("_: %%\n_: a\ns: %s\n_: c\ns: %s\nn: %n\nc: %c\nx: %x", "b", "d",
         123ll, 'e', 0xdeadbeefull);
    klog("\n%n", atoi("123456789", 10));
    klog("\n%x", atoi("deadbeef", 16));
    klog("\n%40aaaa%03bbbb\n");

    klog("%00 %10 %20 %30 %40 %50 %60 %70 %80 %90 %A0 %B0 %C0 %D0 %E0 %F0 ");

    memory_set_boot_info(boot_info);

    spin_halt();
}
