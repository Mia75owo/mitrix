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
    fpu_enable();
    keyboard_init();

    asm volatile ("sti");

    debug_tests();


    tty_set_cursor(0);
    tty_clear();

    tty_printf("_: %%\n_: a\ns: %s\n_: c\ns: %s\nn: %n\nc: %c\nx: %x", "b", "d", 123ll, 'e', 0xdeadbeefull);
    tty_printf("\n%n", atoi("123456789", 10));
    tty_printf("\n%x", atoi("deadbeef", 16));
    tty_printf("\n%40aaaa%03bbbb");

    for(;;);
}
