#include "tests/tests.h"
#include "util/types.h"
#include "serial/serial.h"
#include "tty/tty.h"
#include "fpu/fpu.h"
#include "util/debug.h"
#include "keyboard/keyboard.h"
#include "util/mem.h"

void kernel_main() {
    serial_init();
    fpu_init();
    keyboard_init();

    asm volatile ("sti");

    debug_tests();

    tty_reset();

    klog("_: %%\n_: a\ns: %s\n_: c\ns: %s\nn: %n\nc: %c\nx: %x", "b", "d", 123ll, 'e', 0xdeadbeefull);
    klog("\n%n", atoi("123456789", 10));
    klog("\n%x", atoi("deadbeef", 16));
    klog("\n%40aaaa%03bbbb");

    halt();
}
