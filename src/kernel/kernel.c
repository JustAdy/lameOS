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

    
    char name[] = "lameOS - by Adam Snajdr\n";
    
    set_colors(WHITE, BLUE);
    clear_screen();
    alloc_init();
    
    print_string(name);

    bool lameFSInitializationOK = initialize_lameFS();

    if (lameFSInitializationOK == false) print_string("Error while initializing file system. New one created!!!\n");

    char data[2048];

    for (size_t i = 0; i < 2048; i++)
    {
        data[i] =  'a' + (i % 10);
    }
    


    uint16_t start = write_file(data, 1024, "TEST1.TXT ", FILE, 0x69, 0x69);

    add_dir_entry(48, "TEST1.TXT ", start);

    start = write_file(data, 1024, "TEST2.TXT ", FILE, 0x69, 0x69);

    for (size_t i = 0; i < 2048; i++)
    {
        data[i] =  '0' + (i % 10);
    }

    add_dir_entry(48, "TEST2.TXT ", start);

    update_file_data(start, data, 512);

    shell_init();

    shell_loop();

    return;
}
