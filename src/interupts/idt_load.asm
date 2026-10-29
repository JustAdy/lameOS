[BITS 32]

; Loads the IDT defined in 'idtp' into the processor.
; This is declared in C as 'extern void idt_load();'

section .text
global idt_load
extern idtp

idt_load:
    lidt [idtp]
    ret