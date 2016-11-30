/** \file shapemotion.c
 *  \brief This is a simple shape motion demo.
 *  This demo creates two layers containing shapes.
 *  One layer contains a rectangle and the other a circle.
 *  While the CPU is running the green LED is on, and
 *  when the screen does not need to be redrawn the CPU
 *  is turned off along with the green LED.
 */
#include <msp430.h>
#include <stdjp.h>
#include <stdlib.h> 		/* for malloc */
#include <libTimer.h>
#include <lcdutils.h>
#include <lcddraw.h>
#include <shape.h>
#include <buzzer.h>
#include <rand.h>
#include "switches.h"

#define GREEN_LED BIT6


AbRect rect10 = {abRectGetBounds, abRectCheck, {5,5}}; /**< 10x10 rectangle */
AbRect rect3 = {abRectGetBounds, abRectCheck, {3,3}}; /**< 10x10 rectangle */
AbRArrow rightArrow = {abRArrowGetBounds, abRArrowCheck, 30};

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

Layer eLazer = {
  (AbShape *)&rect3,
  {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  0
};

Layer dLazer = {
  (AbShape *)&rect3,
  {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &eLazer
};

Layer cLazer = {
  (AbShape *)&rect3,
  {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &dLazer
};

Layer bLazer = {
  (AbShape *)&rect3,
  {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &cLazer
};

Layer aLazer = {
  (AbShape *)&rect3,
  {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &bLazer
};

Layer rightLazer = {
  (AbShape *)&rect3,
  {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &aLazer
};

Layer topLazer = {
  (AbShape *)&rect3,
  {(screenWidth/2), (5)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  &rightLazer
};

Layer fieldLayer = {		/* playing field as a layer */
  (AbShape *) &fieldOutline,
  {screenWidth/2, screenHeight/2},/**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_BLACK,
  &topLazer
};

Layer layer0 = {		/**< Layer with a red square */
  (AbShape *)&rect10,
  {screenWidth/2, screenHeight/2}, /**< center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_RED,
  &fieldLayer,
};

/** Moving Layer
 *  Linked list of layer references
 *  Velocity represents one iteration of change (direction & magnitude)
 */
typedef struct MovLayer_s {
  Layer *layer;
  Vec2 velocity;
  struct MovLayer_s *next;
} MovLayer;

/*
  Top Arrow
  topBeam
 */
MovLayer elazer = { &eLazer, {0,1}, 0 };
MovLayer dlazer = { &dLazer, {0,1}, 0 };
MovLayer clazer = { &cLazer, {0,1}, 0 };
MovLayer blazer = { &bLazer, {0,1}, 0 };
MovLayer alazer = { &aLazer, {2,1}, 0 };
MovLayer m1lazer = { &rightLazer, {0,1}, &alazer };
MovLayer mlazer = { &topLazer, {1,1}, &m1lazer };

/* initial value of {0,0} will be overwritten */
MovLayer ml0 = { &layer0, {0,0}, &mlazer };


















//
typedef struct killers_s {
  MovLayer *mov;
  int timer;
  int halfTimer;
  int forthTimer;
  char killMode;
  struct killers_s *next;
} killer;

killer e = { &elazer, 0,0,0,0, 0};
killer d = { &dlazer, 0,0,0,0, 0};
killer c = { &clazer, 0,0,0,0, 0};
killer b = { &blazer, 0,0,0,0, 0};
killer a = { &alazer, 0,0,0,0, 0};
killer r = { &m1lazer, 0,0,0,0, &a};
killer t = { &mlazer, 0,0,0,0, &r};
// void movLayerAdd(MovLayer *ml, MovLayer *new) {
//   if( ml->next != 0 )
//     movLayerAdd(ml->next, new);
//   else {
//     ml->next = new;
//     return;
//   }
// }
// void kLayerAdd(killer *ml, killer *new) {
//   for( ; ml->next != 0; ml = ml->next ){}
//   ml->next = new;
// }
// void addKiller() {
//   Layer *temp = (Layer *) malloc( sizeof(Layer) );
//   temp->abShape = (AbShape *)&rect3;
//   temp->pos.axes[0] = (screenWidth/2 + (random() % (screenWidth) - screenWidth/2));
//   temp->pos.axes[1] = (screenHeight/2 + (random() % (screenHeight) - screenHeight/2));
//   temp->posLast.axes[0] = temp->pos.axes[0];
//   temp->posLast.axes[1] = temp->pos.axes[1];
//   temp->posNext.axes[0] = 0;
//   temp->posNext.axes[1] = 0;
//   temp->color = COLOR_WHITE;
//   temp->next = 0;
//   MovLayer *temp_ml = (MovLayer *) malloc( sizeof(MovLayer) );
//
//   temp_ml->layer = temp;
//   temp_ml->velocity.axes[0] = random() % 6 + 1;
//   temp_ml->velocity.axes[1] = random() % 6 + 1;
//   temp_ml->next = 0;
//
//   killer *temp_k = (killer *) malloc( sizeof(killer) );
//
//   movLayerAdd(&ml0,temp_ml);
//   temp_k->mov = temp_ml;
//   temp_k->timer = ((random() % 10) + 1) * 250;
//   temp_k->halfTimer = temp_k->timer /2;
//   temp_k->forthTimer = temp_k->halfTimer /2;
//   temp_k->killMode = 0;
//   temp_k->next = 0;
//
//   // initKiller(temp_k, temp_ml);
//   // kLayerAdd(&a,temp_k);
// }

void movLayerDraw(MovLayer *movLayers, Layer *layers)
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
        for (probeLayer = layers; probeLayer; probeLayer = probeLayer->next) { /* probe all layers, in order */
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

    //check axes[0]
    if(shapeBoundary.topLeft.axes[0] < fence->topLeft.axes[0]) {
      newPos.axes[0] += (fence->topLeft.axes[0] - shapeBoundary.topLeft.axes[0]);
      ml->velocity.axes[0] = -ml->velocity.axes[0];
    } else if(shapeBoundary.botRight.axes[0] > fence->botRight.axes[0]) {
      newPos.axes[0] -= (shapeBoundary.botRight.axes[0] - fence->botRight.axes[0]);
      ml->velocity.axes[0] = -ml->velocity.axes[0];
    }

    if(shapeBoundary.topLeft.axes[1] < fence->topLeft.axes[1]) {
      newPos.axes[1] += (fence->topLeft.axes[1] - shapeBoundary.topLeft.axes[1]);
      ml->velocity.axes[1] = -ml->velocity.axes[1];
    } else if(shapeBoundary.botRight.axes[1] > fence->botRight.axes[1]) {
      newPos.axes[1] -= (shapeBoundary.botRight.axes[1] - fence->botRight.axes[1]);
      ml->velocity.axes[1] = -ml->velocity.axes[1];
    }




    //check axes[1]
    // if(shapeBoundary.topLeft.axes[1] < fence->topLeft.axes[1])
    //   newPos.axes[1] += (fence->topLeft.axes[1] - shapeBoundary.topLeft.axes[1]);
    // else if(shapeBoundary.topLeft.axes[1] > fence->topLeft.axes[1])
    //   newPos.axes[1] -= (shapeBoundary.topLeft.axes[1] - fence->topLeft.axes[1]);

    // for (axis = 0; axis < 2; axis ++) {
    //   if( )
    //   if ((shapeBoundary.topLeft.axes[axis] < fence->topLeft.axes[axis]) || (shapeBoundary.botRight.axes[axis] > fence->botRight.axes[axis]) ) {
    //     int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
    //     newPos.axes[axis] += (2*velocity);
    //   }	/**< if outside of fence */
    // } /**< for axis */
    ml->layer->posNext = newPos;
  } /**< for ml */
}




u_int bgColor = COLOR_BLUE;     /**< The background color */
int redrawScreen = 1;           /**< Boolean for whether screen needs to be redrawn */

Region fieldFence;		/**< fence around playing field  */
Region topFence;















/** Initializes everything, enables interrupts and green LED,
 *  and handles the rendering for the screen
 */
void main()
{
  P1DIR |= GREEN_LED;		/**< Green led on when CPU on */
  P1OUT |= GREEN_LED;

  configureClocks();
  lcd_init();
  sw_init();
  buzzer_init();
  rand_init();

  layerInit(&layer0);
  layerDraw(&layer0);
  // addKiller();


  layerGetBounds(&fieldLayer, &fieldFence);
  // layerGetBounds(&topFenceLayer, &topFence);


  enableWDTInterrupts();      /**< enable periodic interrupt */
  or_sr(0x8);	              /**< GIE (enable interrupts) */


  for(;;) {
    while (!redrawScreen) { /**< Pause CPU if screen doesn't need updating */
      P1OUT &= ~GREEN_LED;    /**< Green led off witHo CPU */
      or_sr(0x10);	      /**< CPU OFF */
    }
    P1OUT |= GREEN_LED;       /**< Green led on when CPU on */
    redrawScreen = 0;
    movLayerDraw(&ml0, &layer0);
  }
}
unsigned long score = 0;

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;
  if (count == 10) {
    count = 0;
    redrawScreen = 1;

    char buffer [33];
    itoa (score,buffer);
    buffer[32] = '\0';

    drawString5x7(0,0, buffer, COLOR_GREEN, COLOR_RED);

    if(sw1down)
      (&ml0)->velocity.axes[1] = -5;
    else if(sw2down)
      (&ml0)->velocity.axes[1] = 5;
    else
      (&ml0)->velocity.axes[1] = 0;

    if(sw3down)
      (&ml0)->velocity.axes[0] = -5;
    else if(sw4down)
      (&ml0)->velocity.axes[0] = 5;
    else
      (&ml0)->velocity.axes[0] = 0;


    mlAdvance(&ml0, &fieldFence);
    // mlAdvance(&mlazer, &topFence);
    // if (!sw1down)
    //   redrawScreen = 1;
  }
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
