extern handler_additional_info

isr_common:
    push ecx
    ; save additional handler info
    mov ecx, cr2
    mov [handler_additional_info], ecx
    mov ecx, [esp + 12]
    mov [handler_additional_info + 4], ecx
    ; restore ecx
    pop ecx

    ; struct CPUState
    push ebp
    push edi
    push esi

    push edx
    push ecx
    push ebx
    push eax

    push ds
    push es
    push fs
    push gs

    ; load kernel data segment
    push eax
    mov ax, 0x10
    mov ds, ax
    mov es, ax
    mov fs, ax
    mov gs, ax
    pop eax

    ; u32 handle_interrupt(CPUState* frame)
    ;                                ^^^^^
    push esp

    extern handle_interrupt
    call handle_interrupt

    ; return value (new stack pointer)
    ; mov esp, eax
    add esp, 4

global isr_exit
isr_exit:
    pop gs
    pop fs
    pop es
    pop ds

    ; CPUState
    pop eax
    pop ebx
    pop ecx
    pop edx

    pop esi
    pop edi
    pop ebp

    add esp, 8

    iret

%macro ISR_ERROR_CODE 1
global isr%1
isr%1:
    push dword %1
    jmp isr_common
%endmacro

%macro ISR_NO_ERROR_CODE 1
global isr%1
isr%1:
    push dword 0
    push dword %1
    jmp isr_common
%endmacro

ISR_NO_ERROR_CODE 0
ISR_NO_ERROR_CODE 1
ISR_NO_ERROR_CODE 2
ISR_NO_ERROR_CODE 3
ISR_NO_ERROR_CODE 4
ISR_NO_ERROR_CODE 5
ISR_NO_ERROR_CODE 6
ISR_NO_ERROR_CODE 7
ISR_ERROR_CODE    8
ISR_NO_ERROR_CODE 9
ISR_ERROR_CODE    10
ISR_ERROR_CODE    11
ISR_ERROR_CODE    12
ISR_ERROR_CODE    13
ISR_ERROR_CODE    14
ISR_NO_ERROR_CODE 15
ISR_NO_ERROR_CODE 16
ISR_NO_ERROR_CODE 17
ISR_NO_ERROR_CODE 18
ISR_NO_ERROR_CODE 19
ISR_NO_ERROR_CODE 20
ISR_NO_ERROR_CODE 21
ISR_NO_ERROR_CODE 22
ISR_NO_ERROR_CODE 23
ISR_NO_ERROR_CODE 24
ISR_NO_ERROR_CODE 25
ISR_NO_ERROR_CODE 26
ISR_NO_ERROR_CODE 27
ISR_NO_ERROR_CODE 28
ISR_NO_ERROR_CODE 29
ISR_NO_ERROR_CODE 30
ISR_NO_ERROR_CODE 31

ISR_NO_ERROR_CODE 32
ISR_NO_ERROR_CODE 33
ISR_NO_ERROR_CODE 34
ISR_NO_ERROR_CODE 35
ISR_NO_ERROR_CODE 36
ISR_NO_ERROR_CODE 37
ISR_NO_ERROR_CODE 38
ISR_NO_ERROR_CODE 39
ISR_NO_ERROR_CODE 40
ISR_NO_ERROR_CODE 41
ISR_NO_ERROR_CODE 42
ISR_NO_ERROR_CODE 43
ISR_NO_ERROR_CODE 44
ISR_NO_ERROR_CODE 45
ISR_NO_ERROR_CODE 46
ISR_NO_ERROR_CODE 47

global isr_redirect_table
isr_redirect_table:
%assign i 0
%rep 48
    dd isr%+i
%assign i i+1
%endrep
