#include "LEDAnimations.hpp"

LEDAnimations::LEDAnimations() {

}

//This is a quick demo hack
void LEDAnimations::ledanimate(int onoff) {
  if (onoff) {
   for (int i=0;i<NUM_LEDS_PER_WINDOW;i++) {
    leds_windows[0][i] =  CRGB::ForestGreen;
   }
  } else {
   for (int i=0;i<NUM_LEDS_PER_WINDOW;i++) {
    leds_windows[0][i] =  CRGB::Black;
   }
  } 
  
  FastLED.show();
}

void LEDAnimations::ledanimate() {
 //NOT sure yet
  
  FastLED.show();
}

void LEDAnimations::setup() {
      FastLED.addLeds<WS2812, DATA_PIN_WALL_1, GRB>(leds_windows[0], NUM_LEDS_PER_WINDOW);
}