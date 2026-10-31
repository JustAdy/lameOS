#include <stdint.h>
#include <stddef.h>
#include <kernel/system.h>
#include <kernel/string.h>
#include <io/vga_terminal.h>
#include <io/keyboard.h>
#include <shell/shell.h>
#include <io/disk_driver.h>
#include <kernel/lamefs.h>
#include <bool.h>

extern void main()
{
    // set up IDT
    idt_install();

    keyboard_init();

    alloc_init();

    char name[] = "lameOS - by Adam Snajdr\n";

    set_colors(WHITE, BLUE);
    clear_screen();

    print_string(name);

    bool lameFSInitializationOK = initialize_lameFS();

    if (lameFSInitializationOK == false) print_string("Error while initializing file system. New one created!!!\n");

    shell_init();

    shell_loop();

    return;
}
