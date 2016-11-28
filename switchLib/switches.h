#ifndef switches_included
#define switches_included

#define SW ( BIT0 | BIT1 | BIT2 | BIT3 )
extern char sw1down, sw2down, sw3down, sw4down;

void switch_init();
void switch_interrupt_handler();

#endif
