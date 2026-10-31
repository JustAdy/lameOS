#ifndef VGA_TERMINAL_H
#define VGA_TERMINAL_H
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

void set_colors(uint8_t foreground, uint8_t background);

void update_cursor(uint8_t x, uint8_t y);

void put_char(char c);

void print_string(char* string);

void print_long(long number);

void clear_screen();

#endif