#include "idt.h"
#include "util/port.h"
#include "util/mem.h"
#include "util/debug.h"

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

#define PIC1           0x20
#define PIC2           0xA0
#define PIC1_COMMAND   PIC1
#define PIC2_COMMAND   PIC2
#define PIC1_DATA      (PIC1 + 1) // 0x21
#define PIC2_DATA      (PIC2 + 1) // 0xA1

#define PIC_EOI        0x20       // end of input

#define ICW1_ICW4      0x01
#define ICW1_SINGLE    0x02
#define ICW1_INTERVAL4 0x04
#define ICW1_LEVEL     0x08
#define ICW1_INIT      0x10

#define ICW4_8086      0x01

void setup_irq() {
    u8 a1, a2;
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4); // 0x11
    outb(PIC1_DATA, 32);                       // offset
    outb(PIC1_DATA, 0x04);                     // slave PIC at IRQ2
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC1_DATA, a1);

    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4); // 0x11
    outb(PIC2_DATA, 40);                       // offset
    outb(PIC2_DATA, 0x02);                     // slave
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC2_DATA, a2);
}

void handle_timer(InterruptFrame* frame) { (void)frame; }

void idt_init() {
    idtr.limit = 0x0FFF;
    idtr.ptr = IDT;

    memset(isr_functions, 0, sizeof(isr_functions));
    set_isr_function(32, handle_timer);

    setup_irq();

    for (int i = 0; i < 48; i++) {
        idt_set_entry(idtr.ptr, i, isr_redirect_table[i], TA_INTERRUPT);
    }

    asm ("lidt %0" : : "m" (idtr));
}

void handle_interrupt(InterruptFrame* frame) {
    if (frame->interrupt >= 32 && frame->interrupt <= 47) {
        if (frame->interrupt >= 40) {
            outb(PIC2, PIC_EOI);
        }
        outb(PIC1, PIC_EOI);
    }

    ISRFunction isr = isr_functions[frame->interrupt];
    if (isr == 0) {
        static char buf[32];
        itoa(buf, frame->interrupt, 32, 10);

        kpanic("ERROR: unhandled interrupt ", buf, "!");
    }
    isr(frame);

}
