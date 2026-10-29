#include <stdint.h>
#include <stddef.h>
#include <system.h>
#include <vga_terminal.h>
#include <keyboard.h>

extern void main()
{

    // set up IDT
    idt_install();

    keyboard_init();

    char name[] = "lameOS - by Adam Snajdr\n";

    set_colors(WHITE, BLUE);
    clear_screen(BLUE);

    print_string(name);

    while(1)
    {
        put_char(keyboard_getchar());
    }

    return;
}
