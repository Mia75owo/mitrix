#include "interrupts.h"

#define SCREEN_X 80
#define SCREEN_Y 25

static char* screen = (char*)0xb8000;

__attribute__((no_caller_saved_registers)) void screen_clear() {
    for (int i = 0; i < SCREEN_X * SCREEN_Y * 2; i++) {
        screen[i] = i;
    }
}
__attribute__((no_caller_saved_registers)) void screen_print(char* text) {
    for (int i = 0; *text != '\0'; i += 2, text++) {
        screen[i] = *text;
        screen[i + 1] = 4;
    }
}

__attribute__((interrupt)) void page_fault_handler(struct interrupt_frame* frame) {
    (void)frame;

    screen_clear();
    screen_print("Page Fault >:(");

    for (;;) {
        asm ("cli");
        asm ("hlt");
    }
}
