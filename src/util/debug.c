#include "debug.h"

#include <stdarg.h>

#include "gfx/gfx.h"
#include "gfx/vtty.h"
#include "serial/serial.h"
#include "gfx/tty.h"
#include "util/sys.h"

void klog(char* format, ...) {
    va_list va;
    va_start(va, format);

    tty_vprintf(format, va);
    serial_vprintf(format, va);

    va_end(va);
}

// TODO: rewrite to GFX
void kpanic(const char* format, ...) {
    tty_clear();
    gfx_fill(0xffff0000);
    tty_set_color(0x40);

    va_list va;
    va_start(va, format);

    tty_vprintf(format, va);
    serial_vprintf(format, va);

    va_end(va);

    vtty_render();

    abort();
}
