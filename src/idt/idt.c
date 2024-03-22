#include <stdint.h>
#include "interrupts.h"

#define TA_INTERRUPT 0b10001110
#define TA_CALL      0b10001100
#define TA_TRAP      0b10001111

typedef struct {
    uint16_t offset0;
    uint16_t selector;
    uint8_t _;          // reserved
    uint8_t type_attr;
    uint16_t offset1;
} __attribute__((packed)) IDTDescEntry ;

typedef struct {
    uint16_t limit;
    IDTDescEntry* ptr;
} __attribute__((packed)) IDTR;

void idt_set_offset(IDTDescEntry* idt, uint32_t offset) {
    idt->offset0 = (uint16_t)((offset & 0x0000ffff) >> 0);
    idt->offset1 = (uint16_t)((offset & 0xffff0000) >> 16);
}
uint32_t idt_get_offset(IDTDescEntry* idt) {
    uint32_t offset = 0;
    offset |= (uint32_t)idt->offset0 << 0;
    offset |= (uint32_t)idt->offset1 << 16;
    return offset;
}

IDTR idtr;
IDTDescEntry IDT[256];

void prepare_idt() {
    idtr.limit = 0x0FFF;
    idtr.ptr = IDT;

    IDTDescEntry* int_page_fault = &idtr.ptr[0xE];
    idt_set_offset(int_page_fault, (uint32_t)page_fault_handler);
    int_page_fault->type_attr = TA_INTERRUPT;
    int_page_fault->selector = 0x08;

    asm ("lidt %0" : : "m" (idtr));
}
