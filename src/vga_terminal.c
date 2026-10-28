#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_BUFFER ((volatile uint16_t*)0xB8000)

static size_t row = 0;
static size_t column = 0;

void put_char(char c, uint8_t foreground, uint8_t background)
{
    VGA_BUFFER[row * VGA_WIDTH + column] = background << 12 | foreground << 8 | c;

    if(++column == VGA_WIDTH)
    {
        column = 0;
        row++;
    }
}

void print_string(char string[], uint8_t foreground, uint8_t background)
{
    int index = 0;
    while (string[index] != 0)
    {
        switch (string[index])
        {
            case '\n':
                column = 0;
                row++;
                break;
            
            case '\t':
                column += 4;
                break;

            case '\r':
                column = 0;
                break;
        
            default:
                put_char(string[index], foreground, background);
                break;
        }
        index++;
    }
}

void clear_screen(uint8_t background)
{
    for (size_t row = 0; row < VGA_HEIGHT; row++)
    {
        for (size_t col = 0; col < VGA_WIDTH; col++)
        {
            VGA_BUFFER[row * VGA_WIDTH + col] = background << 12 | '\0';
        }
    }

    row = 0;
    column = 0;
}

