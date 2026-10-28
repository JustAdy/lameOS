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

    outb(0x43, 0x36);

    outb(0x40, 0xFF); // Low byte of 65535
    outb(0x40, 0xFF); // High byte of 65535

    return;
}
