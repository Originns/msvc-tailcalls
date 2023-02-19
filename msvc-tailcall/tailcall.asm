extrn pOriginal : qword
extrn pHookfunc : proc
.data
.code

tailStub proc
    ; save general purpose register arguments in shadow space, push xmm registers
    ;
    ;     _______________________
    ;    |          ...          |
    ;    |_______________________| <----- hook stackframe begin ^
    ;    |                       |
    ;    |        padding        |
    ;    |_______________________|  ____
    ;    |                       |      |
    ;    |         xmm0d         |      |
    ;    |_______________________|      |
    ;    |                       |      |
    ;    |         xmm1d         |      |
    ;    |_______________________|      | 32 bytes for lower halves of xmm registers
    ;    |                       |      |
    ;    |         xmm2d         |      |
    ;    |_______________________|      |
    ;    |                       |      |
    ;    |         xmm3d         |      |
    ;    |_______________________|  ____| <--- rsp at function begin
    ;    |                       |
    ;    |       ret address     |
    ;    |_______________________|  ____  <--- 16 byte alignment
    ;    |                       |      |
    ;    |          rcx          |      |
    ;    |_______________________|      |
    ;    |                       |      |
    ;    |          rdx          |      |
    ;    |_______________________|      | shadowspace
    ;    |                       |      |
    ;    |          r8           |      |
    ;    |_______________________|      |
    ;    |                       |      |
    ;    |          r9           |      |
    ;    |_______________________|  ____|
    ;    |                       |
    ;    |  arguments on stack   |
    ;    |_______________________| <----- our stackframe begin ^
    ;    |          ...          |
    ;    |_______________________|
    ;
    ;

    ; backup general purpose register passed arguments in shadowspace
    mov     qword ptr[rsp + 8h], rcx
    mov     qword ptr[rsp + 10h], rdx
    mov     qword ptr[rsp + 18h], r8
    mov     qword ptr[rsp + 20h], r9
    
    ; make space for shadowspace and xmm registers
    sub     rsp, 48h

    ; backup xmm registers
    movsd   qword ptr[rsp + 28h], xmm0
    movsd   qword ptr[rsp + 30h], xmm1
    movsd   qword ptr[rsp + 38h], xmm2
    movsd   qword ptr[rsp + 40h], xmm3

    ; pass pointer to stack
    lea     rcx, [rsp + 28h]
    
    ; call hook
    call    pHookfunc

    ; restore xmm registers
    movsd   xmm3, qword ptr[rsp + 40h]
    movsd   xmm2, qword ptr[rsp + 38h]
    movsd   xmm1, qword ptr[rsp + 30h]
    movsd   xmm0, qword ptr[rsp + 28h]

    ; tear down stack frame
    add     rsp, 48h

    ; restore general purpose register arguments
    mov     r9, qword ptr[rsp + 20h]
    mov     r8, qword ptr[rsp + 18h]
    mov     rdx, qword ptr[rsp + 10h]
    mov     rcx, qword ptr[rsp + 8h]

    jmp     pOriginal
    int     3
tailStub endp

end