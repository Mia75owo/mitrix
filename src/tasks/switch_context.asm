; void switch_context(Task* old, Task* next);
global switch_context
switch_context:
    mov eax, [esp + 4] ; eax = old
    mov edx, [esp + 8] ; edx = next

    ; save old TaskReturnContext
    push ebp
    push ebx
    push esi
    push edi

    ; save esp
    mov [eax], esp ; old->kesp = esp
    ; load new esp
    mov esp, [edx] ; esp = next->kesp

    ; change page dir
    mov eax, [edx + 8]
    sub eax, 0xC0000000
    mov cr3, eax

    ; load new TaskReturnContext
    pop edi
    pop esi
    pop ebx
    pop ebp

    ret
