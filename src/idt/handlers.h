#ifndef HANDLERS_H_
#define HANDLERS_H_

#include "idt.h"

typedef struct {
    u32 cr2;
    u32 eip;
} HandlerAdditionalInfo;

void handler_page_fault(CPUState* frame);
void handler_general_protection_fault(CPUState* frame);

#endif
