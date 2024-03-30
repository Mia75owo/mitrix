#include "util/types.h"
#include "idt.h"
#include "interrupts.h"


IDTR idtr;
IDTDescEntry IDT[256];
ISRFunction isr_functions[256];

extern void* isr_redirect_table[];

void idt_set_entry(IDTDescEntry* idt, u8 index, void* isr, u8 attributes) {
    idt[index].offset0 = (u16)(((u32)isr & 0x0000ffff) >> 0);
    idt[index].offset1 = (u16)(((u32)isr & 0xffff0000) >> 16);
    idt[index].type_attr = attributes;
    idt[index].selector = 0x08;
}

void set_isr_function(u8 index, ISRFunction func) {
    isr_functions[index] = func;
}

void prepare_idt() {
    idtr.limit = 0x0FFF;
    idtr.ptr = IDT;

    for (int i = 0; i < 48; i++) {
        idt_set_entry(idtr.ptr, i, isr_redirect_table[i], TA_INTERRUPT);
    }

    asm ("lidt %0" : : "m" (idtr));
}
void handle_interrupt(InterruptFrame* frame) {
    (void)frame;
}
