#include <stdint.h>
#include <stddef.h>
#include <system.h>
#include <vga_terminal.h>

extern void main()
{

    // set up IDT
    idt_install();

    char name[] = "lameOS - by Adam Snajdr\n";

    set_colors(WHITE, BLUE);
    clear_screen(BLUE);

    print_string(name);

    return;
}
