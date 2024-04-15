#include "fpu/fpu.h"
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

    tty_reset();

    kinit(gdt_load(), "GDT");
    kinit(idt_load(), "IDT");
    kinit(serial_init(), "Serial");
    kinit(fpu_init(), "FPU");
    kinit(keyboard_init(), "Keyboard");
    kinit(pit_init(1000), "PIT");
    u32 mod1 = *(u32*)(boot_info->mods_addr + 4);
    u32 physical_alloc_start = (mod1 + 0xFFF) & ~0xFFF;

    // bool graphics_enabled = boot_info->framebuffer_type == MULTIBOOT_FRAMEBUFFER_TYPE_RGB;
    klog("\n%n\n", (u64)boot_info->framebuffer_type);
    klog("\n%n\n", (u64)boot_info->framebuffer_width);
    klog("\n%n\n", (u64)boot_info->framebuffer_height);
    klog("\n%n\n", (u64)boot_info->framebuffer_bpp);
    klog("\n%n\n", (u64)boot_info->framebuffer_pitch);

    kinit(memory_init(boot_info->mem_upper * 1024, physical_alloc_start),
          "Memory");

    asm volatile("sti");

    /*debug_tests();*/

    klog("_: %%\n_: a\ns: %s\n_: c\ns: %s\nn: %n\nc: %c\nx: %x", "b", "d",
         123ll, 'e', 0xdeadbeefull);
    klog("\n%n", atoi("123456789", 10));
    klog("\n%x", atoi("deadbeef", 16));
    klog("\n%40aaaa%03bbbb\n");

    memory_set_boot_info(boot_info);

    spin_halt();
}
