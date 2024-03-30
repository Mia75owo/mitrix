#include "util/types.h"
#include "fpu/fpu.h"

static char* screen = (char*)0xB8000;

void print(char* text) {
    for (u16 i = 0; *text != '\0'; i += 2, text++) {
        screen[i] = *text;
        screen[i + 1] = 4;
    }
}

void kernel_main() {
    print("Hello from kernel!");

    fpu_enable();

    for (;;)
        ;
}
