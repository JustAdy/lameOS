#include <stddef.h>
#include <stdint.h>
#include <io/keyboard.h>
#include <io/vga_terminal.h>
#include <kernel/system.h>
#include <kernel/string.h>
#include <shell/programs/programs.h>

#define BUFFER_SIZE 128

char* command_buffer;
uint8_t index;

void execute_command(uint8_t index);

void shell_init()
{
    command_buffer = malloc(BUFFER_SIZE);
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
                free(command_buffer, BUFFER_SIZE);
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
                command_buffer[index] = '\0';
                execute_command(index);
                waiting_for_command = 0;
                break;
            
            default:
                if (index < BUFFER_SIZE)
                {
                    command_buffer[index] = c;
                    put_char(c);
                    index++;
                }
                break;
            }
        }
        
    }
    
}

void execute_command(uint8_t index)
{
    size_t endIndex = 0;

    for (size_t i = 0; i < index; i++)
    {
        char c = command_buffer[i];
        if (c == ' ')
        {
            endIndex = i;
            break;
        }
    }

    if (memcmp(command_buffer, "echo", 4) == 0) echo(command_buffer, endIndex + 1);
    else if (memcmp(command_buffer, "clear", 5) == 0) clear_screen();
    else print_string("\nInvalid input!\n");
}