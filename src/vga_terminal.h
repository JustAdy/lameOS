#include <stdint.h>
#include <stddef.h>

#define BLACK 0
#define BLUE 1
#define GREEN 2
#define CYAN 3
#define RED 4
#define MAGENTA 5
#define BROWN 6
#define GRAY 7
#define DARK_GRAY 8
#define LIGHT_BLUE 9
#define LIGHT_GREEN 10
#define LIGHT_CYAN 11
#define LIGHT_RED 12
#define LIGHT_MAGENTA 13
#define YELLOW 14
#define WHITE 15

void put_char(char c, uint8_t foreground, uint8_t background);

void print_string(char* string, uint8_t foreground, uint8_t background);

void clear_screen(uint8_t background);