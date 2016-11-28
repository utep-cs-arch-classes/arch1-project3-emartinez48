#include <libTimer.h>
#include <stdio.h>
#include <stdlib.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "shape.h"
#include "rand.h"

char* itoa(int i, char b[]){
  char const digit[] = "0123456789";
  char* p = b;
  if(i<0){
      *p++ = '-';
      i *= -1;
  }
  int shifter = i;
  do{ //Move to where representation ends
      ++p;
      shifter = shifter/10;
  }while(shifter);
  *p = '\0';
  do{ //Move back, inserting digits as u go
      *--p = digit[i%10];
      i = i/10;
  }while(i);
  return b;
}

main()
{
  configureClocks();
  lcd_init();
  rand_init();
  Vec2 rectPos = screenCenter;

  clearScreen(COLOR_BLUE);

  unsigned int v = random() % 5; // return 0 inclusive to 5 exclusive
  char buffer [33];

  itoa (v,buffer);
  buffer[32] = '\0';


  drawString5x7(20,20, buffer, COLOR_GREEN, COLOR_RED);

}
