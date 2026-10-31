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

static inline void outb(uint16_t port, uint8_t val)
{
    __asm__ volatile ("out dx, al" : : "d"(port), "a"(val));
};

static inline uint8_t inb(uint16_t port)
{
    uint8_t ret;
    __asm__ volatile ("in al, dx" : "=a"(ret) :"d"(port));
    return ret;
}

static inline void outw(uint16_t port, uint16_t val)
{
    __asm__ volatile ("out dx, ax" : : "d"(port), "a"(val));
};

static inline uint16_t inw(uint16_t port)
{
    uint16_t ret;
    __asm__ volatile ("in ax, dx" : "=a"(ret) :"d"(port));
    return ret;
}

void idt_install(void);

void pic_unmask_irq(uint8_t irq);

void alloc_init();

void* malloc(size_t bytes);

void free(void* ptr, size_t bytes);

#endif