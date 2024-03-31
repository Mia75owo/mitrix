extern kernel_main

SECTION .multiboot
align 4

%define MAGIC 0x1badb002
%define ALIGN 1<<0
%define MEMINFO 1<<1
%define FLAGS (ALIGN | MEMINFO)
%define CHECKSUM (-(MAGIC + FLAGS))

dd MAGIC
dd FLAGS
dd CHECKSUM

SECTION .bss
align 16

stack_bottom:
resb 16384
stack_top:


SECTION .text

%include "./src/gdt/gdt.asm"
%include "./src/idt/load_idt.asm"
%include "./src/idt/interrupts_defines.asm"

global _start
_start:
    mov esp, stack_top

    ;int 0x0e                ; trigger "page fault" to test IDT

    push ebx
    push eax

    cli
    call kernel_main

halt:
    cli
    hlt
    jmp halt
