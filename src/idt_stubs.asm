[BITS 32]

section .text
global divide_stub
divide_stub:
    push 0 ; dummy
    push 0 ; vector
    jmp common_ISR

global common_ISR
extern ISR
common_ISR:
    pushad
    push esp
    call ISR
    add esp, 4
    popad
    add esp, 8
    iret