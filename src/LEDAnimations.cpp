#include "LEDAnimations.hpp"

LEDAnimations::LEDAnimations() {
  windows[0] = Window();
}

void LEDAnimations::draw() {
 //NOT sure yet
  FastLED.show();
}

void LEDAnimations::setup() {
      FastLED.addLeds<WS2812, DATA_PIN_WALL_1, GRB>(entire_thing[0], TOTAL_LEDS);
}

void LEDAnimations::process_window(uint8_t index)
{
  if (windows[index].on_fire)
  {
    windows[index].fake_fire();
  }
  else
  {
    windows[index].blackout_window();
  }
  uint16_t offset = index * STRANDS_PER_WINDOW * LEDS_PER_STRAND;
  windows[index].cp_data(entire_thing[0], offset);
}

void LEDAnimations::process_all_windows()
{
  uint8_t lenWindows = sizeof(windows)/sizeof(windows[0]);
  for (int i=0; i<lenWindows; i++)
  {
    process_window(i);
  }
}

Window::Window()
{

}

void Window::blackout_window()
{
  for (int i=0; i < strands; i++)
  {
    for (int j=0; j < leds_per_strand; j++)
    {
      pixels[i][j] = CRGB::Black;
    }
  }
}

void Window::setup()
{
  blackout_window();
}

uint32_t Window::crgb_to_hex(CRGB color)
{
  return ((long)color.r << 16L) | ((long)color.g << 8L) | (long)color.b;
}

void Window::fake_fire()
//set arbitrary height, copy in num flames...
{
  for (int i=0; i < strands;i++)
  {
    bool reverse = false;
    if (i % 2 != 0)
    {
      reverse = true;
    }
    //calculate a random height for the flame for this strand
    uint8_t height = random(leds_per_strand/4, leds_per_strand);
    for( int j = 0; j < leds_per_strand; j++) {
      CRGB color;
      //calculate the offset to start copying in the flames.
      uint8_t lenFlames = sizeof(flames_colors) / sizeof(flames_colors[0]);
      uint8_t offset = lenFlames - height;
      if (j <= height)
      {
        color = flames_colors[j+offset];
      }
      else
      {
        color = CRGB::Black;
      }
      int pixelnumber;
      if( reverse ) {
        pixelnumber = (leds_per_strand-1) - j;
      } else {
        pixelnumber = j;
      }
      // Serial.println(crgb_to_hex(color), HEX);
      pixels[i][pixelnumber] = color;
    }
  }
}

void Window::cp_data(CRGB* buffer, uint16_t offset) 
{
  int k = offset;
  for (int i=0; i < strands; i++)
  {
    for( int j = 0; j < leds_per_strand; j++)
    {
      buffer[k] = pixels[i][j];
      k++;
    }
  }
}

void LEDAnimations::loop()
{
  unsigned long now = millis();
  if (now - last_render_time > 70)
  {
    process_all_windows();
    draw();
    last_render_time = now;
  }

}