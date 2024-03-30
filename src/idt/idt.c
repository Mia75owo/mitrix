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
} __attribute__((packed)) IDTDescEntry ;

typedef struct {
    u16 limit;
    IDTDescEntry* ptr;
} __attribute__((packed)) IDTR;

void idt_set_offset(IDTDescEntry* idt, u32 offset) {
    idt->offset0 = (u16)((offset & 0x0000ffff) >> 0);
    idt->offset1 = (u16)((offset & 0xffff0000) >> 16);
}
u32 idt_get_offset(IDTDescEntry* idt) {
    u32 offset = 0;
    offset |= (u32)idt->offset0 << 0;
    offset |= (u32)idt->offset1 << 16;
    return offset;
}

IDTR idtr;
IDTDescEntry IDT[256];

void prepare_idt() {
    idtr.limit = 0x0FFF;
    idtr.ptr = IDT;

    IDTDescEntry* int_page_fault = &idtr.ptr[0xE];
    idt_set_offset(int_page_fault, (u32)page_fault_handler);
    int_page_fault->type_attr = TA_INTERRUPT;
    int_page_fault->selector = 0x08;

    asm ("lidt %0" : : "m" (idtr));
}
