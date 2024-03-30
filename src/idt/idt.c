#include "util/types.h"
#include "interrupts.h"

#define TA_INTERRUPT 0b10001110
#define TA_CALL      0b10001100
#define TA_TRAP      0b10001111

typedef struct {
    u16 offset0;
    u16 selector;
    u8 _;          // reserved
    u8 type_attr;
    u16 offset1;
} __attribute__((packed)) IDTDescEntry;

typedef struct {
    u16 limit;
    IDTDescEntry* ptr;
} __attribute__((packed)) IDTR;

void idt_set_entry(IDTDescEntry* idt, u8 index, void* isr, u8 attributes) {
    idt[index].offset0 = (u16)(((u32)isr & 0x0000ffff) >> 0);
    idt[index].offset1 = (u16)(((u32)isr & 0xffff0000) >> 16);
    idt[index].type_attr = attributes;
    idt[index].selector = 0x08;
}

IDTR idtr;
IDTDescEntry IDT[256];

void prepare_idt() {
    idtr.limit = 0x0FFF;
    idtr.ptr = IDT;

    idt_set_entry(idtr.ptr, 0xE, page_fault_handler, TA_INTERRUPT);

    asm ("lidt %0" : : "m" (idtr));
}
