extern prepare_idt

load_idt:
    push ebp          ; save old call frame
    mov ebp, esp      ; set stack base to current stack pointer

    call prepare_idt  ; call c function

    mov esp, ebp      ; restore stack pointer
    pop ebp           ; restore stack base

    ret
