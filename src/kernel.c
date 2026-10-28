#include <stdint.h>
#include <stddef.h>
#include "vga_terminal.h"

extern void main()
{
    char name[] = "lameOS - by Adam Snajdr\n";

    clear_screen(LIGHT_GREEN);

    print_string(name, WHITE, BLUE);
    print_string(name, WHITE, BLUE);

    return;
}
