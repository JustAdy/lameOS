#include <stdint.h>
#include <stddef.h>
#include <kernel/system.h>
#include <kernel/string.h>
#include <io/vga_terminal.h>
#include <io/keyboard.h>
#include <shell/shell.h>
#include <io/disk_driver.h>
#include <kernel/lamefs.h>

extern void main()
{
    // set up IDT
    idt_install();

    keyboard_init();

    alloc_init();

    char name[] = "lameOS - by Adam Snajdr\n";

    set_colors(WHITE, BLUE);
    clear_screen();

    initialize_lameFS();

    print_string(name);

    shell_init();

    shell_loop();

    return;
}
