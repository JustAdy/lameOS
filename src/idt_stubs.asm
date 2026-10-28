[BITS 32]

section .text
global divide_stub
divide_stub:
    push 0 ; dummy
    push 0 ; vector
    jmp common_ISR

global debug_stub
debug_stub:
    push 1 ; dummy
    push 1 ; vector
    jmp common_ISR

global nmi_stub
nmi_stub:
    push 2 ; dummy
    push 2 ; vector
    jmp common_ISR

global breakpoint_stub
breakpoint_stub:
    push 3 ; dummy
    push 3 ; vector
    jmp common_ISR

global overflow_stub
overflow_stub:
    push 4 ; dummy
    push 4 ; vector
    jmp common_ISR

global bound_stub
bound_stub:
    push 5 ; dummy
    push 5 ; vector
    jmp common_ISR

global opcode_stub
opcode_stub:
    push 6 ; dummy
    push 6 ; vector
    jmp common_ISR

global device_stub
device_stub:
    push 7 ; dummy
    push 7 ; vector
    jmp common_ISR

global double_stub
double_stub:
    push 8 ; vector
    jmp common_ISR

global tss_stub
tss_stub:
    push 10 ; vector
    jmp common_ISR

global segment_stub
segment_stub:
    push 11 ; vector
    jmp common_ISR

global stack_stub
stack_stub:
    push 12 ; vector
    jmp common_ISR

global general_stub
general_stub:
    push 13 ; vector
    jmp common_ISR

global page_stub
page_stub:
    push 14 ; vector
    jmp common_ISR

global alignment_stub
alignment_stub:
    push 17 ; vector
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