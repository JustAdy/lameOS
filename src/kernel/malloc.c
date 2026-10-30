#include <stddef.h>
#include <stdint.h>
#include <kernel/system.h>
#include <kernel/string.h>

extern uint8_t __heap_start[];
extern uint8_t __heap_end[];

uint8_t bitmap[2048];

void alloc_init()
{
    memset(bitmap, 0, 2048);
}

void* malloc(size_t bytes)
{
    if (bytes == 0) return NULL;

    size_t blocks = (bytes + 31) / 32;

    size_t count = 0;
    size_t startIndex = 0;

    for (size_t index = 0; index < 2048; index++)
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

    if (index >= 2048 || blocks > 2048 - index) return;   // out of range

    memset(&bitmap[index], 0x00, blocks);
}
