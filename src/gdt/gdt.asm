gdtr:
    dw gdt_end - gdt_base - 1
    dd gdt_base

gdt_base:
    ; null descriptor
    dw 0           ; limit        16    16
    dw 0           ; base         16    32
    db 0           ; base         8     40
    db 0           ; access byte  8     48
    db 0           ; limit 4 | flags 4  56
    db 0           ; base         8     64

    ; code segment
    dw 0xFFFF      ; limit        16    16
    dw 0           ; base         16    32
    db 0           ; base         8     40
    db 0x9A        ; access byte  8     48
    db 0b11001111  ; limit 4 | flags 4  56
    db 0           ; base         8     64

    ; data segment
    dw 0xFFFF      ; limit        16    16
    dw 0           ; base         16    32
    db 0           ; base         8     40
    db 0x92        ; access byte  8     48
    db 0b11001111  ; limit 4 | flags 4  56
    db 0           ; base         8     64
gdt_end:

reload_segments:
    jmp 0x08:.reload_CS
.reload_CS:

    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    mov ss, ax

    ret
