#ifndef IDT_H_
#define IDT_H_

#include "util/types.h"

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

typedef struct {
    u32 gs, fs, es, ds;
    u32 edi, esi, ebp, esp, ebx, edx, ecx, eax;
    u32 interrupt, error;

    u32 eip, cs, eflags, usermode_esp, usermode_ss;
} InterruptFrame;

typedef void (*ISRFunction)(InterruptFrame*);


void set_isr_function(u8 index, ISRFunction func);
void idt_set_entry(IDTDescEntry* idt, u8 index, void* isr, u8 attributes);
void idt_init();

#endif
