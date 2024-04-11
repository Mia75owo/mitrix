#include "idt.h"

#include "util/debug.h"
#include "util/mem.h"
#include "util/port.h"

IDTR idtr;
IDTDescEntry IDT[256];
ISRFunction isr_functions[256];

extern void* isr_redirect_table[];

void idt_set_entry(IDTDescEntry* idt, u8 index, void* isr, u8 attributes) {
    DWord offset;
    offset.val = (u32)isr;
    idt[index].offset0 = offset.lower.val;
    idt[index].offset1 = offset.higher.val;

    idt[index].type_attr = attributes;
    idt[index].selector = 0x08;
}

void set_isr_function(u8 index, ISRFunction func) {
    isr_functions[index] = func;
}

// clang-format off
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
// clang-format on

static void setup_irq() {
    u8 a1, a2;
    a1 = inb(PIC1_DATA);
    a2 = inb(PIC2_DATA);

    outb(PIC1_COMMAND, ICW1_INIT | ICW1_ICW4);  // 0x11
    outb(PIC1_DATA, 32);                        // offset
    outb(PIC1_DATA, 0x04);                      // slave PIC at IRQ2
    outb(PIC1_DATA, ICW4_8086);
    outb(PIC1_DATA, a1);

    outb(PIC2_COMMAND, ICW1_INIT | ICW1_ICW4);  // 0x11
    outb(PIC2_DATA, 40);                        // offset
    outb(PIC2_DATA, 0x02);                      // slave
    outb(PIC2_DATA, ICW4_8086);
    outb(PIC2_DATA, a2);
}

void empty_handle(InterruptFrame* frame) { (void)frame; }

void idt_init() {
    idtr.limit = 0x0FFF;
    idtr.ptr = IDT;

    memset(isr_functions, 0, sizeof(isr_functions));
    set_isr_function(39, empty_handle);

    setup_irq();

    for (int i = 0; i < 48; i++) {
        idt_set_entry(idtr.ptr, i, isr_redirect_table[i], TA_INTERRUPT);
    }

    asm("lidt %0" : : "m"(idtr));
}

static char* exception_names[];

void handle_interrupt(InterruptFrame* frame) {
    if (frame->interrupt >= 32 && frame->interrupt <= 47) {
        if (frame->interrupt >= 40) {
            outb(PIC2, PIC_EOI);
        }
        outb(PIC1, PIC_EOI);
    }

    ISRFunction isr = isr_functions[frame->interrupt];
    if (isr == 0) {
        if (frame->interrupt < 32) {
            kpanic("ERROR: Exception (%s)!", exception_names[frame->interrupt]);
        } else {
            kpanic("ERROR: unhandled interrupt %n!", (u64)frame->interrupt);
        }
    }
    isr(frame);
}

static char* exception_names[] = {
    "Division Error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "Reserved",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Reserved",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    "Reserved",
};
