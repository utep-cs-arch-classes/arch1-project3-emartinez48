#include <libTimer.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdjp.h>
#include "lcdutils.h"
#include "lcddraw.h"
#include "shape.h"
#include "rand.h"


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
