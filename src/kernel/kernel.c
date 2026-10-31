#include <stdint.h>
#include <stddef.h>
#include <kernel/system.h>
#include <kernel/string.h>
#include <io/vga_terminal.h>
#include <io/keyboard.h>
#include <shell/shell.h>
#include <io/disk_driver.h>

extern void main()
{
    // set up IDT
    idt_install();

    keyboard_init();

    alloc_init();

    char name[] = "lameOS - by Adam Snajdr\n";

    set_colors(WHITE, BLUE);
    clear_screen();

    //print_string(name);

    char write[512];

    char read[512];

    write[0] = 'l';
    write[1] = 'a';
    write[2] = 'm';
    write[3] = 'e';
    write[4] = 'O';
    write[5] = 'S';
    write[6] = '\0';

    write_sector(64, (uint16_t*)write);
    read_sector(64, (uint16_t*)read);

    print_string(read);

    shell_init();

    shell_loop();

    return;
}
