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
unsigned long score = 0;
unsigned int health = 200;
unsigned int reset_countdown = 250*3;
unsigned long top_score = 0;
int buzzer_current_count = 0;

AbRect rect10 = {abRectGetBounds, abRectCheck, {5,5}}; /**< 10x10 rectangle */
AbRect rect3 = {abRectGetBounds, abRectCheck, {3,3}}; /**< 10x10 rectangle */
AbRArrow rightArrow = {abRArrowGetBounds, abRArrowCheck, 30};

AbRectOutline fieldOutline = {	/* playing field */
  abRectOutlineGetBounds, abRectOutlineCheck,
  {screenWidth/2 - 10, screenHeight/2 - 10}
};

// Layer cLazer = {
//   (AbShape *)&rect3,
//   {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
//   {0,0}, {0,0},				    /* last & next pos */
//   COLOR_WHITE,
//   0
// };

Layer bLazer = {
  (AbShape *)&rect3,
  {(screenWidth-5), (screenHeight/2)}, /**< bit below & right of center */
  {0,0}, {0,0},				    /* last & next pos */
  COLOR_WHITE,
  0
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
// MovLayer clazer = { &cLazer, {0,1}, 0 };
MovLayer blazer = { &bLazer, {1,2}, 0 };
MovLayer alazer = { &aLazer, {2,1}, &blazer };
MovLayer m1lazer = { &rightLazer, {0,1}, &alazer };
MovLayer mlazer = { &topLazer, {1,1}, &m1lazer };

/* initial value of {0,0} will be overwritten */
MovLayer ml0 = { &layer0, {0,0}, &mlazer };


















//
// typedef struct killers_s {
//   MovLayer *mov;
//   int timer;
//   int halfTimer;
//   int forthTimer;
//   char killMode;
//   struct killers_s *next;
// } killer;
//
// // killer c = { &clazer, 0,0,0,0, 0};
// killer b = { &blazer, 0,0,0,0, 0};
// killer a = { &alazer, 0,0,0,0, &b};
// killer r = { &m1lazer, 0,0,0,0, &a};
// killer t = { &mlazer, 0,0,0,0, &r};
//

// from https://developer.mozilla.org/en-US/docs/Games/Techniques/2D_collision_detection
void checkCollision() {
  Vec2 newPos;
  Region shapeBoundary;
  Region shapeBoundary2;
  MovLayer *ml = &mlazer;

  int ax = 0;
  int ay = 0;
  int aX = 0;
  int aY = 0;

  int bx = ml0.layer->pos.axes[0] - 10;
  int by = ml0.layer->pos.axes[1] - 10;
  int bX = ml0.layer->pos.axes[0] + 10;
  int bY = ml0.layer->pos.axes[1] + 10;

  char buffer [33];
  buffer[32] = '\0';

  for (; ml; ml = ml->next) {
    vec2Add(&newPos, &ml->layer->posNext, &ml->velocity);
    ax = ml->layer->posNext.axes[0] - 2;
    ay = ml->layer->posNext.axes[1] - 2;
    aX = ml->layer->posNext.axes[0] + 2;
    aY = ml->layer->posNext.axes[1] + 2;
    if( !(aX<bx || bX<ax || aY<by || bY<ay)) {
      char axis;
      for ( axis = 0; axis < 2; axis ++) {
        int velocity = ml->velocity.axes[axis] = -ml->velocity.axes[axis];
        newPos.axes[axis] += (2*velocity);
      } /**< for axis */
      ml->layer->posNext = newPos;
      health -= 1 * score/3 + 1;
      if( !buzzer_on) {
        buzzer_current_count = 0;
        // sound(_B,2);
      }
    }
  }
}

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

/** Watchdog timer interrupt handler. 15 interrupts/sec */
void wdt_c_handler()
{
  static short count = 0;
  P1OUT |= GREEN_LED;		      /**< Green LED on when cpu on */
  count ++;


  if( health > 200 || health <= 0 ) {
    buzzer_set_period(1000);
    if( reset_countdown > 0 ) {
      reset_countdown -= 1;
    }

    // you lose
    // reset game
    if (score > top_score)
      top_score = score;
    // score = 0;
    // health = 200;
    if( reset_countdown > 100 ) {
      redrawScreen = 0;
      reset_countdown -= 1;
      drawString5x7(0,50, "YOU LOSE", COLOR_GREEN, COLOR_RED);
      drawString5x7(0,60, "SCORE: ", COLOR_GREEN, COLOR_RED);
      char buffer [33];
      itoa (score,buffer);

      drawString5x7(screenWidth/2,60, buffer, COLOR_GREEN, COLOR_RED);
      drawString5x7(0,90, "PREPARE FOR RESET", COLOR_GREEN, COLOR_RED);

      P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
    } else if( reset_countdown == 0 ) {



      health = 200;
      ml0.layer->pos.axes[0] = random() % screenWidth;
      ml0.layer->pos.axes[1] = random() % screenHeight;
      ml0.layer->posNext.axes[0] = ml0.layer->pos.axes[0];
      ml0.layer->posNext.axes[1] = ml0.layer->pos.axes[1];
      drawString5x7(0,50, "YOU LOSE", COLOR_BLUE, COLOR_BLUE);
      drawString5x7(0,60, "SCORE: ", COLOR_BLUE, COLOR_BLUE);
      drawString5x7(screenWidth/2,60, "000000", COLOR_BLUE, COLOR_BLUE);
      drawString5x7(0,90, "PREPARE FOR RESET", COLOR_BLUE, COLOR_BLUE);
      score = 0;
      count = 0;
      buzzer_set_period(0);
    }
    return;
  }

  if( count == 250 ) {
    count = 0;
    score += 1;
  }

  if (count %20 == 0) {
    redrawScreen = 1;

    char buffer [33];
    itoa (score,buffer);

    drawString5x7(0,0, buffer, COLOR_GREEN, COLOR_BLUE);
    wipe(buffer,33);

    itoa(health,buffer);
    drawString5x7(0,screenHeight-7, "___\0", COLOR_GREEN, COLOR_RED);
    drawString5x7(0,screenHeight-7, buffer, COLOR_GREEN, COLOR_RED);

    wipe(buffer,33);
    itoa(top_score,buffer);
    drawString5x7(screenWidth-5*5,screenHeight-7, buffer, COLOR_GREEN, COLOR_RED);

    // drawString5x7(20,20, "Health\0", COLOR_GREEN, COLOR_RED);

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


    checkCollision();
    mlAdvance(&ml0, &fieldFence);
    // mlAdvance(&mlazer, &topFence);
    // if (!sw1down)
    //   redrawScreen = 1;
  }
  P1OUT &= ~GREEN_LED;		    /**< Green LED off when cpu off */
}
