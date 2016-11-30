/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */  
#include <msp430.h>
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <p2switches.h>
#include <shape.h>
#include <abCircle.h>
#include "buzzer.h"

#define GREEN_LED BIT6



AbRect rect10 = {abRectGetBounds, abRectCheck, {5,20}}; 


AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,   
  {screenWidth/2 - 3, screenHeight/2 - 15}
};


Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  0
};


Layer layer2 = {		/**< Modified to make our paddle */
  (AbShape *)&rect10,
  {8, 70}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};


Layer layer1 = {		/**< Modified to make our paddle */
  (AbShape *)&rect10,
  {116, 70}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLUE,
  &layer2,
};



Layer layer0 = {	      
  (AbShape *)&circle10,
  {20, 30}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &layer1,
};

char Points[2];

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/* initial value of {0,0} will be overwritten */

MovLayer ml2 = {&layer2, {0,0},0};
MovLayer ml1 = {&layer1, {0,0},&ml2};  /**< not all layers move */
MovLayer ml0 = {&layer0, {1,2}, &ml1}; 


movLayerDraw(MovLayer *movLayers, Layer *layers)
{
  int row, col;
  MovLayer *movLayer;

  and_sr(~8);			/**< disable interrupts (GIE off) */
  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Layer *l = movLayer->layer;
    l->posLast = l->pos;
    l->pos = l->posNext;
  }
  or_sr(8);			/**< disable interrupts (GIE on) */


  for (movLayer = movLayers; movLayer; movLayer = movLayer->next) { /* for each moving layer */
    Region bounds;
    layerGetBounds(movLayer->layer, &bounds);
    lcd_setArea(bounds.topLeft.axes[0], bounds.topLeft.axes[1], 
		bounds.botRight.axes[0], bounds.botRight.axes[1]);
    for (row = bounds.topLeft.axes[1]; row <= bounds.botRight.axes[1]; row++) {
      for (col = bounds.topLeft.axes[0]; col <= bounds.botRight.axes[0]; col++) {
	Vec2 pixelPos = {col, row};
	u_int color = bgColor;
	Layer *probeLayer;
	for (probeLayer = layers; probeLayer; 
	     probeLayer = probeLayer->next) { /* probe all layers, in order */
	  if (abShapeCheck(probeLayer->abShape, &probeLayer->pos, &pixelPos)) {
	    color = probeLayer->color;
	    break; 
	  } /* if probe check */
	} // for checking all layers at col, row
	lcd_writeColor(color); 
      } // for col
    } // for row
  } // for moving layer being updated
}	  
//NOTE I RECEIVED ASSITANCE IN THE CODE BELOW
void intersect(Region  *rIntercet, const Region *FR , const Region *SR)
{
 
 vec2Max(&rIntercet->topLeft, &FR->topLeft, &SR->topLeft);
 vec2Min(&rIntercet->botRight, &FR->botRight, &SR->botRight);
}
  
int touch(const Layer *layer0, const Layer *layer1)
{
   Region Ball;
   Region Paddle;
   Region Union;;

   abShapeGetBounds(layer0->abShape, &(layer0->pos), &Ball);
   abShapeGetBounds(layer1->abShape, &(layer1->pos), &Paddle);
   intersect(&Union,&Ball,&Paddle);
   
   int x1= Union.topLeft.axes[0];
   int x2= Union.botRight.axes[0];
   int y1= Union.topLeft.axes[1];
   int y2= Union.botRight.axes[1];

   if(x1>=x2 || y1>=y2)
     {
       return 0;
     }
   return 1;
}



//Region fence = {{10,30}, {SHORT_EDGE_PIXELS-10, LONG_EDGE_PIXELS-10}}; /**< Create a fence region */

/** Advances a moving shape within a fence
 *  
 *  \param ml The moving shape to be advanced
 *  \param fence The region which will serve as a boundary for ml
 */
void mlAdvance(MovLayer *ml, Region *fence)
{
  Vec2 newPos;
  u_char axis;
  Region shapeBoundary;
  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    abShapeGetBounds(ml->layer->abShape, &newPos, &shapeBoundary);
    for (axis = 0; axis < 2; axis ++) {
      if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) ||
	  (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
	int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
	newPos.axes[axis] += (2*velocity);
      }	/**< if outside of fence */
      if(touch(&layer0,&layer1))
	{
	  int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
          newPos.axes[axis] += (2*velocity);
	  PADDLE();
	}
      if(touch(&layer0,&layer2))
	{
	  int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
         newPos.axes[axis] += (2*velocity);
	 PADDLE();
	}
      
    } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}


u_int bgColor = COLOR_YELLOW;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */

void buzzer_init()
{
    /* 
       Direct timer A output "TA0.1" to P2.6.  
        According to table 21 from data sheet:
          P2SEL2.6, P2SEL2.7, anmd P2SEL.7 must be zero
          P2SEL.6 must be 1
        Also: P2.6 direction must be output
    */
    timerAUpmode();		/* used to drive speaker */
    P2SEL2 &= ~(BIT6 | BIT7);
    P2SEL &= ~BIT7; 
    P2SEL |= BIT6;
    P2DIR = BIT6;		/* enable output to speaker (P2.6) */
    buzzer_set_period(0);	/* start buzzing!!! */
}
WALL()
{
  buzzer_set_period(2000);
}
Regular()
{
  buzzer_set_period(0);
}
PADDLE()
{
 buzzer_set_period(500);
}

void buzzer_set_period(short cycles)
{
  CCR0 = cycles; 
  CCR1 = cycles >> 1;		/* one half cycle */
}





/** Initializes everything, enables interrupts and green LED, 
 *  and handles the rendering for the screen
 */
void main()
{
  
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */	       
  P1OUT |= GREEN_LED;

  
  configureClocks();
  lcd_init();
  shapeInit();
  p2sw_init(15);
  shapeInit();
  layerInit(&layer0);
  layerDraw(&layer0);
  layerGetBounds(&fieldLayer, &fieldFence); 
  enableWDTInterrupts();      /**< enable periodic interrupt */
  buzzer_init();
  or_sr(0x8);	              /**< GIE (enable interrupts) */
 
 Points[0]='0';
 Points[1]='0';
 Points[2]='0';
 char String [6] = "Score:";
  for(;;) { 
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
    
     drawString5x7(5,5,String, COLOR_BLUE,COLOR_YELLOW);
     drawString5x7(40,5,Points, COLOR_BLUE,COLOR_YELLOW);

  }

 }

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
    if(Points[2]==':')
   {
   Points[2]='0';
   Points[1]+=1;
   }
   if(Points[1]==':')
   {
   Points[1]='0';
   Points[0]+=1;
   }

   switch(Points[1])
     {
   case '1':
   bgColor = COLOR_PINK;
   //   rect10.halfSize.axes[1]=10;
   break;
   case '2':
   bgColor = COLOR_SEA_GREEN;
   break;
   case '3':
   bgColor = COLOR_KHAKI;
   break;
   case '4':
   bgColor = COLOR_DARK_ORANGE;
   break;
   case '5':
   bgColor = COLOR_GRAY;
   break;
   case '6':
   bgColor = COLOR_ORANGE;
   break;
   case '7':
   bgColor = COLOR_TURQUOISE;
   break;
   case '8':
   bgColor = COLOR_DARK_GREEN;
   break;
   case '9':
   bgColor = COLOR_FOREST_GREEN;
   break;
   default: 
     bgColor = COLOR_YELLOW;
     // rect10.halfSize.axes[1]=20;
     }

  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 15) {
    mlAdvance(&ml0, &fieldFence);
    ml1.velocity.axes[1]=0;
    ml2.velocity.axes[1]=0;
    Regular();

    if(layer0.pos.axes[0]==17)
      {
	WALL();
	Points[2]='0';
	Points[1]='0';
      }
     if(layer0.pos.axes[0]==107)
      {
	WALL();
	Points[2]='0';
	Points[1]='0';
      }
      if(layer0.pos.axes[1]==30)
      {
	WALL();
    
      }
      if(layer0.pos.axes[1]==130)
      {
	WALL();
      }

    if (!(p2sw_read() & BIT0))
      {
	Points[2]+=1;
	ml1.velocity.axes[1]=-3;
	ml2.velocity.axes[1]=-3;
      }
    if (!(p2sw_read() & BIT1))
      {
	Points[2]+=1;
	ml1.velocity.axes[1]=3;
	ml2.velocity.axes[1]=3;
      }
    if (!(p2sw_read() & BIT2))
      {
	ml0.velocity.axes[0]-=1;
      }
     if (!(p2sw_read() & BIT3))
      {
	ml0.velocity.axes[0]+=1;
      }
    count = 0;
    redrawScreen = 1;
  } 
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
