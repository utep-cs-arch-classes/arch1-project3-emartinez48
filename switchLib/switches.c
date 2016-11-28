#include <msp430.h>
#include "switches.h"
char sw1down, sw2down, sw3down, sw4down, swval;

static void update_sw()
{
  swval = P2IN;
  /* update switch interrupt sensitivity */
  P2IES |= (swval & SW);	/* if switch up, sense down */
  P2IES &= (swval | ~SW);	/* if switch down, sense up */
}

void sw_init()			/* setup switch */
{
  P2REN |= SW;		/* enables resistors for switches */
  P2IE = SW;		/* enable interrupts from switches */
  P2OUT |= SW;		/* pull-ups for switches */
  P2DIR &= ~SW;		/* set switches' bits for input */
  update_sw();
}

void sw_itr_han()
{
  update_switch();
  sw1down = !( swval & 1 );
  sw2down = !( swval & 2 );
  sw3down = !( swval & 4 );
  sw4down = !( swval & 8 );
}

__interrupt(PORT2_VECTOR) Port_2(){
  if( P2IFG & SW ) {	      /* did a button cause this interrupt? */
    P2IFG &= ~SW;		      /* clear pending sw interrupts */
    sw_itr_han();	/* single handler for all switches */
  }
}
