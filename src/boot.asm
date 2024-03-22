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

global _start
_start:
    mov esp, stack_top

    ; Load global descriptor table
    lgdt [gdtr]
    call reload_segments

    mov [0xb8010], byte 'H'
    mov [0xb8012], byte 'i'

    mov [0xb8011], byte 'a' ; color red
    mov [0xb8013], byte 'a' ; color red

    cli
    call kernel_main

halt:
    cli
    hlt
    jmp halt
