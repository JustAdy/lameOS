#include <stdint.h>
#include <stddef.h>

#define VGA_WIDTH  80
#define VGA_HEIGHT 25
#define VGA_BUFFER ((volatile uint16_t*)0xB8000)

static size_t row = 0;
static size_t column = 0;

static uint8_t attribute = 0;

void set_colors(uint8_t foreground, uint8_t background)
{
    attribute = background << 4 | foreground;
}

void put_char(char c)
{
    VGA_BUFFER[row * VGA_WIDTH + column] = attribute << 8 | c;

    if(++column == VGA_WIDTH)
    {
        column = 0;
        row++;
    }
}

void print_string(char string[])
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
                put_char(string[index]);
                break;
        }
        index++;
    }
}

void print_long(long number)
{
    if (number < 0)
    {
        put_char('-');
        number *= -1;
    }

    if (number != 0)
    {
        char reversed_str[11];
        char mod = 0;
        char index = 1;
        reversed_str[0] = '\0';
        while(number)
        {
            mod = number % 10;
            mod += 0x30;
            reversed_str[index++] = mod;
            number /= 10;
        }
    
        index--;
        
        while(index)
        {
            put_char(reversed_str[index--]);
        }
    }

    else if (number == 0)
    {
        put_char('0');
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

