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

    char data[] = "This is yet another text file\n";

    uint16_t start = write_file(data, sizeof(data), "TEST2.TXT ", FILE, 0x69, 0x69);

    add_dir_entry(48, "TEST2.TXT ", start);

    shell_init();

    shell_loop();

    return;
}
