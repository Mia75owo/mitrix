extern idt_init

global idt_load
idt_load:
    push ebp          ; save old call frame
    mov ebp, esp      ; set stack base to current stack pointer

    call idt_init     ; call c function

    mov esp, ebp      ; restore stack pointer
    pop ebp           ; restore stack base

    ret
