#include "util/types.h"
#include "util/serial.h"
#include "tty/tty.h"
#include "fpu/fpu.h"
#include "util/debug.h"
#include "keyboard/keyboard.h"

void kernel_main() {
    tty_clear();
    tty_set_cursor(0);

    tty_color(18, 0x04);
    tty_puts("Hello from kernel!\n");
    serial_init();

    fpu_enable();

    keyboard_init();

    asm volatile ("sti");

    u64 nums[] = {
        0, 9, 123, 987654321
    };

    for (u16 i = 0; i < sizeof(nums) / sizeof(nums[0]); i++) {
        tty_color(32, 0x03);
        tty_put_num(nums[i], 10);
        tty_puts("\n");
    }

    u64 nums_hex[] = {
        0x0, 0x123, 0xdeadbeef, 0xffffffffffffffff
    };

    for (u16 i = 0; i < sizeof(nums_hex) / sizeof(nums_hex[0]); i++) {
        tty_color(32, 0x03);
        tty_puts("0x");
        tty_put_num(nums_hex[i], 16);
        tty_puts("\n");
    }

    assert_msg('o' == 'o', "yay");
    /*assert_msg('o' == 'x', "ohnu");*/

    tty_set_cursor(0);
    tty_clear();

    for (;;)
        ;
}
