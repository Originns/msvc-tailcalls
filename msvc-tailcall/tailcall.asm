extrn pOriginal : qword
extrn pHookfunc : proc
.data
.code

tailStub proc
    ; backup register arguments in shadow space
    mov [rsp + 8h], rcx
    mov [rsp + 10h], rdx
    mov [rsp + 18h], r8
    mov [rsp + 20h], r9

    lea rcx, [rsp + 8h]
    lea rdx, [rsp + 10h]
    lea r8, [rsp + 18h]
    lea r9, [rsp + 20h]

    push rsp

    sub rsp, 20h

    call pHookfunc

    add rsp, 28h

    mov r9, [rsp + 20h]
    mov r8, [rsp + 18h]
    mov rdx, [rsp + 10h]
    mov rcx, [rsp + 8h]

    jmp pOriginal
    int 3
tailStub endp

end