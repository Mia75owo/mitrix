#ifndef IDT_H_
#define IDT_H_

#include "util/types.h"

// clang-format off
#define TA_INTERRUPT 0b10001110
#define TA_CALL      0b10001100
#define TA_TRAP      0b10001111
// clang-format on

typedef struct {
    u16 offset0;
    u16 selector;
    u8 _;  // reserved
    u8 type_attr;
    u16 offset1;
} __attribute__((packed)) IDTDescEntry;

typedef struct {
    u16 limit;
    IDTDescEntry* ptr;
} __attribute__((packed)) IDTR;

typedef struct {
    u32 eax;
    u32 ebx;
    u32 ecx;
    u32 edx;

    u32 esi;
    u32 edi;
    u32 ebp;

    u32 gs;
    u32 fs;
    u32 es;
    u32 ds;

    u32 interrupt;
    u32 error;

    // Pushed by CPU
    u32 eip;
    u32 cs;
    u32 eflags;
    u32 esp;
    u32 ss;
} __attribute__((packed)) CPUState;

typedef void (*ISRFunction)(CPUState*);

void set_isr_function(u8 index, ISRFunction func);
void idt_set_entry(IDTDescEntry* idt, u8 index, void* isr, u8 attributes);
void idt_init();

#endif
