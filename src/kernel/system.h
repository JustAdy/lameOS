#ifndef SYSTEM_H
#define SYSTEM_H

#include <stdint.h>
#include <stddef.h>

void idt_set_gate(
    unsigned char num,
    unsigned long base,
    unsigned short sel,
    unsigned char flags
);

static inline void outb(uint16_t port, uint8_t value)
{
    __asm__ volatile ("outb %0, %1"
                      :
                      : "a"(value), "Nd"(port));
};

static inline uint8_t inb(uint16_t port)
{
    uint8_t value;
    __asm__ volatile ("inb %1, %0" : "=a"(value) : "Nd"(port));
    return value;
}

void idt_install(void);

void pic_unmask_irq(uint8_t irq);

#endif