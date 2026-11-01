#include <stddef.h>
#include <stdint.h>
#include <kernel/system.h>
#include <kernel/string.h>

#define BITMAP_SIZE (2048)

extern uint8_t __heap_start[];
extern uint8_t __heap_end[];

uint8_t bitmap[BITMAP_SIZE];

void alloc_init()
{
    memset(bitmap, 0, BITMAP_SIZE);
}

void* malloc(size_t bytes)
{
    if (bytes == 0) return NULL;

    size_t blocks = (bytes + 31) / 32;

    size_t count = 0;
    size_t startIndex = 0;

    for (size_t index = 0; index < BITMAP_SIZE; index++)
    {
        if (bitmap[index] == 0)
        {
            if (count == 0) startIndex = index;
            count++;

            if (count == blocks)
            {
                for (size_t j = 0; j < blocks; j++)
                {
                    bitmap[startIndex + j] = 0xff;
                }
                return (char*)__heap_start + startIndex * 32;
            }
        }
        else
        {
            count = 0;
        }
    }

    return NULL;
}

void free(void* ptr, size_t bytes)
{
    if (ptr == NULL || bytes == 0) return;

    size_t offset = (char*)ptr - (char*)__heap_start;

    if (offset % 32 != 0) return;           // not a block boundary

    size_t index  = offset / 32;
    size_t blocks = (bytes + 31) / 32;

    if (index >= BITMAP_SIZE || blocks > BITMAP_SIZE - index) return;   // out of range

    memset(&bitmap[index], 0x00, blocks);
}
