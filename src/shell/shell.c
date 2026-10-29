#include <stddef.h>
#include <stdint.h>
#include <io/keyboard.h>
#include <io/vga_terminal.h>
#include <kernel/system.h>
#include <kernel/string.h>

char* command_buffer;
uint8_t index;

void execute_comman();

void shell_init()
{
    command_buffer = malloc(128);
}

void shell_loop()
{
    uint8_t waiting_for_command = 1;
    while (1)
    {
        print_string("lameOS>");
        waiting_for_command = 1;
        index = 0;
        while (waiting_for_command)
        {
            char c = keyboard_getchar();
            switch (c)
            {
            case '\e':
                return;

            case '\b':
                if (index <= 0) 
                {
                    index = 0;
                }
                else
                {
                    put_char('\b');
                    index--;
                }
                
                break;

            case '\n':
                execute_comman();
                waiting_for_command = 0;
                break;
            
            default:
                put_char(c);
                index++;
                break;
            }
        }
        
    }
    
}

void execute_comman()
{
    print_string("Executing command...\n");
}