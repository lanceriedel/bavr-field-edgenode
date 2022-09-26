#ifndef LEDAnimations_h
#define LEDAnimations_h


#include <FastLED.h>



#define DATA_PIN_WALL_1 5
#define NUM_LEDS_PER_WINDOW 20

class LEDAnimations
{
public:
    LEDAnimations();
    void setup();
    void ledanimate(int onoff);
    void ledanimate();

private:
    //for lights
    CRGB leds_windows[1][NUM_LEDS_PER_WINDOW];
};

#endif
