#include <FastLED.h>

/*
------------------------
|                      |
|<--*^     *v     *^   |
|                      |
|   *^     *v     *^   |
|                      |
|   *^     *v     *^   |
|                      |
|   *^     *v     *^   |
|                      |<--
------------------------

12 pixels = 1 window
24 pixels = 2 windows = 1 strip (1 data pin)

Windows are arrays

Gutter is a separate array

Fire2012 is a function that is called to set up the fire animation frames
*/ 

//#define DEBUG 1

// Constants 
#define NUM_WALLS 2
#define NUM_LEDS_PER_WALL 24
#define NUM_LEDS_PER_WINDOW 12

#define NUM_GUTTERS 1 
#define NUM_LEDS_PER_GUTTER 30

#define DATA_PIN_WALL_0 11
#define DATA_PIN_WALL_1 12
#define DATA_PIN_GUTTER_0 13

#define MAX_FS 8
#define MAX_TOTAL_WINDOWS 4

// Animation times
#define BURN_RATE 3000  // ms
#define BLINK_TIME 500  // ms
#define FRAMES_PER_SECOND 40 // fps

// Variables
uint8_t FIRE_SCORE = 0; 
int thisLoopWindowCount = 0; 
int theMSthatHavePassed = 0;
int theTargetBlinkMS = BLINK_TIME; 
bool blinkOn = false;
int theTargetBurnMS = BURN_RATE;  // start with the burn_rate as the target  

// Window arrays - tell it which pixels are in each window
// A better version of this would dynamically build these arrays based on NUM_LEDS_PER_WINDOW
uint8_t window_0[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
uint8_t window_1[] = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};

// Create FastLED arrays of arrays 
CRGB leds_wall[NUM_WALLS][NUM_LEDS_PER_WALL];
CRGB leds_gutter[NUM_GUTTERS][NUM_LEDS_PER_GUTTER];

void setup() {
  // Setup debugger
  #ifdef DEBUG
  Serial.begin(9600);
  #endif

  // Create FastLED arrays for walls
  FastLED.addLeds<WS2812, DATA_PIN_WALL_0, GRB>(leds_wall[0], NUM_LEDS_PER_WALL);
  FastLED.addLeds<WS2812, DATA_PIN_WALL_1, GRB>(leds_wall[1], NUM_LEDS_PER_WALL);

  // Create FastLED arrays for gutters
  FastLED.addLeds<NEOPIXEL, DATA_PIN_GUTTER_0>(leds_gutter[0], NUM_LEDS_PER_GUTTER);
}

void loop() {
  //-------/
  // - Each second, the gutter will blink on and off to show time has passed
  // - Every 2 seconds, the FS will increase
  // - For every FS, a window will light up, and the gutter will increase 
  //-------/

  // --- Initialize variables 
  int theDeleteScore = 0;


  //------------------------------------------------//
  // ----------------- FIRE SCORE ----------------- // 
  //------------------------------------------------//
  // --- If we've passed the new burn MS, update the fire score and target burn MS 
  if (theMSthatHavePassed >= theTargetBurnMS) { 
    // - Only update if we haven't reached Max FS    
    if (FIRE_SCORE < MAX_FS ) {
      FIRE_SCORE++;
      theTargetBurnMS = theTargetBurnMS + BURN_RATE;
    }
    else {
      theTargetBurnMS = 99999999;
    }
  }

  //------------------------------------------------//
  // ------------------- WINDOWS ------------------ // 
  //------------------------------------------------//
  // --- Loop walls - run Fire2012 for each full wall (i.e. build the animation "frame")
  for (int wall = 0; wall < NUM_WALLS; wall++) {
    Fire2012_WholeWall(wall);
  }

  // --- Un-do non-active windows - start with the highest first
  // - this is really wonky and was written after an unspecified number of Truly seltzers
  if (FIRE_SCORE <= MAX_TOTAL_WINDOWS) { //only do this whole thing if you have the right num windows 
    theDeleteScore = MAX_TOTAL_WINDOWS - FIRE_SCORE;
    for (int wall = (NUM_WALLS - 1); wall >= 0; wall--) { 
      for (int window = 1; window >= 0; window--) {
        // Check the window
        if (thisLoopWindowCount < theDeleteScore) {
          // If we're still in blackout range, black out the window
          // This is either silly or magical it's up to you to decide
          for (int pixel = 0; pixel < NUM_LEDS_PER_WINDOW; pixel++) {
            if (window == 1) {
              leds_wall[wall][window_1[pixel]] = CRGB::Black;
            }
            else if (window == 0) { 
              leds_wall[wall][window_0[pixel]] = CRGB::Black;
            }
          }
        }
        // Next window
        thisLoopWindowCount++;
      }
    }
    //Reset "delete window count" for the next loop
    thisLoopWindowCount = 0;
  }

  //------------------------------------------------//
  // ------------------- GUTTERS ------------------ // 
  //------------------------------------------------//
  // --- Loop gutters
  // -- If it's time to blink, check which blink state to switch and update
  if (theMSthatHavePassed >= theTargetBlinkMS) {
    if (blinkOn == true) {
      // Loop gutter LEDs for Yellah
      for (int gutter = 0; gutter < NUM_GUTTERS; gutter++) {
        for (int pixel = (NUM_LEDS_PER_GUTTER - 5); pixel < NUM_LEDS_PER_GUTTER; pixel++) {
          leds_gutter[gutter][pixel] = CRGB::Yellow;
        }
      }
      // Reset blinker
      blinkOn = false;
    }
    else if (blinkOn == false) {
      // Loop gutter LEDs for Blackout
      for (int gutter = 0; gutter < NUM_GUTTERS; gutter++) {
        for (int pixel = (NUM_LEDS_PER_GUTTER - 5); pixel < NUM_LEDS_PER_GUTTER; pixel++) {
          leds_gutter[gutter][pixel] = CRGB::Black;
        }
      }
      // Reset blinker
      blinkOn = true;
    }    
    // Reset timer
    theTargetBlinkMS = theTargetBlinkMS + BLINK_TIME;
  } //end main "if time to blink" IF

  // -- Update the gutters with the current FS
  // Loop gutter LEDs for Red on FS qty
  // (note as currently written could overflow if num_leds < max_fs*2)
  for (int gutter = 0; gutter < NUM_GUTTERS; gutter++) {
    for (int i = 0; i < FIRE_SCORE; i++) {
      for (int pixel = (i*3); pixel <= (i*3+2); pixel++) {
        leds_gutter[gutter][pixel] = CRGB::Red;
      }
    }
  }



  //------------------------------------------------//
  // ------------------- FASTLED ------------------ // 
  //------------------------------------------------//
  // --- Do the thing
  FastLED.show();
  FastLED.delay(1000 / FRAMES_PER_SECOND);
  theMSthatHavePassed = theMSthatHavePassed + (1000 / FRAMES_PER_SECOND);
}

/*
So, I could see a few different avenues here
- Always animate the whole building, then black out the windows that aren't active
- Only animate the windows that are active
- Only animate the walls that are active 

I think doing the whole building may look best?
But it would make the array math pretty odd.. I think it'll be easiest
if it stays as "per wall" (or per whatever "unit of LED strip") 
*/


//------ everything here down is from the Fire2012 example code -------//

// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 50, suggested range 20-100 
#define COOLING  40

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 75

void Fire2012_WholeWall(int whichFireWall){
  bool gReverseDirection = false;

  // GC - special variable that calcs number of pixels for whole building
  int NUM_FIRE_LEDS = NUM_LEDS_PER_WALL;

  // Array of temperature readings at each simulation cell
  uint8_t heat[NUM_FIRE_LEDS];

  // Step 1.  Cool down every cell a little
    for( int i = 0; i < NUM_FIRE_LEDS; i++) {
      heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / NUM_FIRE_LEDS) + 2));
    }
  
    // Step 2.  Heat from each cell drifts 'up' and diffuses a little
    for( int k= NUM_FIRE_LEDS - 1; k >= 2; k--) {
      heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3;
    }
    
    // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
    if( random8() < SPARKING ) {
      int y = random8(7);
      heat[y] = qadd8( heat[y], random8(160,255) );
    }

    // Step 4.  Map from heat cells to LED colors
    for( int j = 0; j < NUM_FIRE_LEDS; j++) {
      CRGB color = HeatColor( heat[j]);
      int pixelnumber;
      if( gReverseDirection ) {
        pixelnumber = (NUM_FIRE_LEDS-1) - j;
      } else {
        pixelnumber = j;
      }
      leds_wall[whichFireWall][pixelnumber] = color;
    }
}
