#ifndef PS2_H
#define PS2_H

void keyboard_init();
void keyboard_irq_handler();
int keyboard_getchar();

#endif