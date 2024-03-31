extern idt_init

load_idt:
    push ebp          ; save old call frame
    mov ebp, esp      ; set stack base to current stack pointer

    call idt_init     ; call c function

    mov esp, ebp      ; restore stack pointer
    pop ebp           ; restore stack base

    ret
