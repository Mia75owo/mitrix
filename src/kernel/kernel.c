#include "tests/tests.h"
#include "util/sys.h"
#include "util/types.h"
#include "serial/serial.h"
#include "tty/tty.h"
#include "fpu/fpu.h"
#include "pit/pit.h"
#include "util/debug.h"
#include "keyboard/keyboard.h"
#include "util/mem.h"
#include "multiboot.h"
#include "memory/memory.h"

extern void gdt_load();
extern void idt_load();

#define kinit(call, name) \
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
    /*kinit(memory_init(boot_info), "Memory");*/

    asm volatile ("sti");

    /*debug_tests();*/

    klog("_: %%\n_: a\ns: %s\n_: c\ns: %s\nn: %n\nc: %c\nx: %x", "b", "d", 123ll, 'e', 0xdeadbeefull);
    klog("\n%n", atoi("123456789", 10));
    klog("\n%x", atoi("deadbeef", 16));
    klog("\n%40aaaa%03bbbb\n");

    u32 mod1 = *(u32*)(boot_info->mods_addr + 4);
    u32 physical_alloc_start = (mod1 + 0xFFF) & ~0xFFF;

    memory_init(boot_info->mem_upper * 1024, physical_alloc_start);

    klog("mem allocation done");

    memory_set_boot_info(boot_info);

    spin_halt();
}
