#ifndef SYSTEM_H
#define SYSTEM_H

void idt_set_gate(
    unsigned char num,
    unsigned long base,
    unsigned short sel,
    unsigned char flags
);

void idt_install(void);

#endif