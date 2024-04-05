extern kernel_main

; ===============================================
; MULTIBOOT
SECTION .multiboot
align 4

%define MBOOT_PAGE_ALIGN (1 << 0)
%define MBOOT_MEMINFO    (1 << 1)
%define MBOOT_USE_GFX    (0)

%define MAGIC 0x1badb002
%define FLAGS (MBOOT_PAGE_ALIGN | MBOOT_MEMINFO | MBOOT_USE_GFX)
%define CHECKSUM (-(MAGIC + FLAGS))

dd MAGIC
dd FLAGS
dd CHECKSUM
dd 0, 0, 0, 0, 0

; graphics
dd 0
dd 800     ; x-size
dd 600     ; y-size
dd 32      ; depth

; ===============================================
; BSS
SECTION .bss
align 16

stack_bottom:
resb 0x20000
stack_top:

; ===============================================
; BOOT
SECTION .boot

global _start
_start:
    mov ecx, (initial_page_dir - 0xC0000000)
    mov cr3, ecx

    ; turn on physical address extension
    ; cr4 |= 0x10
    mov ecx, cr4
    or ecx, 0x10
    mov cr4, ecx

    ; enable paging
    ; cr0 |= 0x80000000
    mov ecx, cr0
    or ecx, 0x80000000
    mov cr0, ecx
    
    jmp higher_half

; ===============================================
; TEXT
SECTION .text

higher_half:
    mov esp, stack_top

    xor ebp, ebp

    push ebx
    push eax

    cli
    call kernel_main

halt:
    cli
    hlt
    jmp halt

%include "./src/gdt/gdt.asm"
%include "./src/idt/load_idt.asm"
%include "./src/idt/interrupts_defines.asm"

; ===============================================
; DATA
SECTION .data
align 4096

global initial_page_dir
initial_page_dir:
    dd 10000011b

    times 768-1 dd 0

    dd (0 << 22) | 10000011b
    dd (1 << 22) | 10000011b
    dd (2 << 22) | 10000011b
    dd (3 << 22) | 10000011b

    times 256-4 dd 0
