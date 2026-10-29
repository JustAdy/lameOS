#include <stdint.h>
#include <stddef.h>
#include <kernel/system.h>
#include <kernel/string.h>
#include <io/vga_terminal.h>
#include <io/keyboard.h>

extern void main()
{
    // set up IDT
    idt_install();

    keyboard_init();

    alloc_init();

    char name[] = "lameOS - by Adam Snajdr\n";

    set_colors(WHITE, BLUE);
    clear_screen(BLUE);

    print_string(name);

    char* str = malloc(128);
    memcpy(str, "This is allocated with malloc()", strlen(str));   // or memcpy(str, "...", 32)

    print_string(str);
    put_char('\n');
    print_long((long)str);       // should print 20480
    put_char('\n');

    char* ptr = malloc(64);
    print_long((long)ptr);       // should print 20608

    free(str, 128);
    free(ptr, 64);

    while(1)
    {
        put_char(keyboard_getchar());
    }

    return;
}
