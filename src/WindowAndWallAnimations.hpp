
#ifndef WindowAndWallAnimations_h
#define WindowAndWallAnimations_h


#include "Arduino.h"



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

class WindowAndWallAnimations
{
public:
    WindowAndWallAnimations();
    void setup();

    void loop();
  

private:
    void  Fire2012_WholeWall(int whichFireWall);



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


// Variables
uint8_t FIRE_SCORE = 0; 
int thisLoopWindowCount = 0; 
int theMSthatHavePassed = 0;
int theTargetBlinkMS = BLINK_TIME; 
bool blinkOn = false;
int theTargetBurnMS = BURN_RATE;  // start with the burn_rate as the target  


// Create FastLED arrays of arrays 
CRGB leds_wall[NUM_WALLS][NUM_LEDS_PER_WALL];
CRGB leds_gutter[NUM_GUTTERS][NUM_LEDS_PER_GUTTER];

// Window arrays - tell it which pixels are in each window
// A better version of this would dynamically build these arrays based on NUM_LEDS_PER_WINDOW
uint8_t window_0[12] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11};
uint8_t window_1[12] = {12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23};


};



#endif