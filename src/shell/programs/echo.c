#include <shell/programs/programs.h>
#include <stddef.h>
#include <stdint.h>
#include <io/vga_terminal.h>

void echo(char* str, uint8_t program_len)
{
    put_char('\n');
    print_string(str + program_len);
    put_char('\n');
}
