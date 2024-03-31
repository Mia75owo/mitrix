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

extern void gdt_load();
extern void idt_load();

#define kinit(call, name) \
    klog("%0F[ %0BINFO %0F] %07Loading %s\n", name); \
    (call);                                          \
    klog("%0F[  %0AOK  %0F] %07%s Success\n", name);

void kernel_main() {
    tty_reset();

    kinit(gdt_load(), "GDT");
    kinit(idt_load(), "IDT");
    kinit(serial_init(), "Serial");
    kinit(fpu_init(), "FPU");
    kinit(keyboard_init(), "Keyboard");
    kinit(pit_init(1000), "PIT");

    asm volatile ("sti");

    /*debug_tests();*/

    klog("_: %%\n_: a\ns: %s\n_: c\ns: %s\nn: %n\nc: %c\nx: %x", "b", "d", 123ll, 'e', 0xdeadbeefull);
    klog("\n%n", atoi("123456789", 10));
    klog("\n%x", atoi("deadbeef", 16));
    klog("\n%40aaaa%03bbbb");

    spin_halt();
}
