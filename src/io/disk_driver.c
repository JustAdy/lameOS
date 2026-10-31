#include <stdint.h>
#include <kernel/system.h>

void read_sector(uint32_t lba, uint16_t *buffer)
{
    while (inb(0x1F7) & 0x80);

    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F7, 0x20);

    while (1)
    {
        uint8_t status = inb(0x1F7);
        if (!(status & 0x80) && (status & 0x08))
        {
            break;
        }
    }
    
    for (size_t i = 0; i < 256; i++)
    {
        buffer[i] = inw(0x1F0);
    }    
}

void write_sector(uint32_t lba, const uint16_t *buffer)
{
    while (inb(0x1F7) & 0x80);

    outb(0x1F2, 1);
    outb(0x1F3, (uint8_t)lba);
    outb(0x1F4, (uint8_t)(lba >> 8));
    outb(0x1F5, (uint8_t)(lba >> 16));
    outb(0x1F6, 0xE0 | ((lba >> 24) & 0x0F));
    outb(0x1F7, 0x30);

    while (1)
    {
        uint8_t status = inb(0x1F7);
        if (!(status & 0x80) && (status & 0x08))
        {
            break;
        }
    }
    
    for (size_t i = 0; i < 256; i++)
    {
        outw(0x1F0, buffer[i]);
    }    

    while (inb(0x1F7) & 0x80);
    outb(0x1F7, 0xE7);
    while (inb(0x1F7) & 0x80);
}
