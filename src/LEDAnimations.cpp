#include "LEDAnimations.hpp"

uint32_t crgb_to_hex(CRGB color)
{
  return ((long)color.r << 16L) | ((long)color.g << 8L) | (long)color.b;
}

Window::Window()
{
}

Window::Window(uint16_t the_first_pixel)
{
  first_pixel = the_first_pixel;
}

void Window::blackout_window()
{
  for (int i = 0; i < strands; i++)
  {
    for (int j = 0; j < leds_per_strand; j++)
    {
      pixels[i][j] = CRGB::Black;
    }
  }
}

void Window::setup()
{
  blackout_window();
}

void Window::fake_fire()
// set arbitrary height, copy in num flames...
{
  for (int i = 0; i < strands; i++)
  {
    bool reverse = false; //this is the reversing function to handle strips that are oriented 180 from each other
    if (i % 2 != 0)
    {
      reverse = true;
    }
    // calculate a random height for the flame for this strand
    uint8_t height = random(leds_per_strand / 4, leds_per_strand);
    for (int j = 0; j < leds_per_strand; j++)
    {
      CRGB color;
      // calculate the offset to start copying in the flames to the strand
      uint8_t lenFlames = sizeof(flames_colors) / sizeof(flames_colors[0]);
      uint8_t offset = lenFlames - height;
      if (j <= height)
      {
        color = flames_colors[j + offset];
      }
      else
      {
        color = CRGB::Black;
      }
      int pixelnumber;
      if (reverse)
      {
        pixelnumber = (leds_per_strand - 1) - j;
      }
      else
      {
        pixelnumber = j;
      }
      // Serial.println(crgb_to_hex(color), HEX);
      pixels[i][pixelnumber] = color; //fill a strand at a time
    }
  }
}

void Window::compute()
{
  if (on_fire)
  {
    fake_fire();
  }
  else
  {
   blackout_window();
  }
}

void Window::cp_data(CRGB *buffer)
{
  int k = first_pixel;
  for (int i = 0; i < strands; i++) //for each strand of the window
  {
    for (int j = 0; j < leds_per_strand; j++) //for each led in the strand
    {
      buffer[k] = pixels[i][j]; //copy in the pixel data to the buffer
      k++;
    }
  }
}

Side::Side()
{
}

Building::Building()
{
}
void Building::set_active_windows(uint8_t side, uint8_t windows)
{
  Serial.print(F("Trying to set window "));
  Serial.print(side);
  Serial.print(F(" to "));
  Serial.println(windows);

  if (side > 0)
  {
    side--; //convert for 0 indexing
    if (side < (sizeof(sides) / sizeof(sides[0])))
    {
      if (windows > 1)
      {
        sides[side].windows[0].on_fire = true;
        sides[side].windows[1].on_fire = true;
      }
      else if (windows == 1)
      {
        sides[side].windows[0].on_fire = true;
        sides[side].windows[1].on_fire = false;
      }
      else
      {
        sides[side].windows[0].on_fire = false;
        sides[side].windows[1].on_fire = false;
      }
    }
  }
}

LEDAnimations::LEDAnimations()
{
  building = Building();
  building.sides[0] = Side();
  building.sides[1] = Side();
  building.sides[2] = Side();
  building.sides[3] = Side();
  building.sides[0].windows[0] = Window(0);
  building.sides[0].windows[1] = Window(1 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
  building.sides[1].windows[0] = Window(2 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
  building.sides[1].windows[1] = Window(3 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
  building.sides[2].windows[0] = Window(4 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
  building.sides[2].windows[1] = Window(5 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
  building.sides[3].windows[0] = Window(6 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
  building.sides[3].windows[1] = Window(7 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
}

void LEDAnimations::draw()
{
  // NOT sure yet
  FastLED.show();
}

void LEDAnimations::setup()
{
  FastLED.addLeds<WS2812, DATA_PIN_WALL_1, GRB>(entire_thing[0], TOTAL_LEDS);
}

void LEDAnimations::process_window(uint8_t side, uint8_t window)
{
  building.sides[side].windows[window].compute();
  building.sides[side].windows[window].cp_data(entire_thing[0]);
}

void LEDAnimations::process_all_windows()
{
  uint8_t lenWindows = sizeof(building.sides[0].windows) / sizeof(building.sides[0].windows[0]);
  uint8_t lenSides = sizeof(building.sides) / sizeof(building.sides[0]);
  for (int i = 0; i < lenSides; i++)
  {
    for (int j = 0; j < lenWindows; j++)
    {
      process_window(i, j);
    }
  }
}
void LEDAnimations::loop()
{
  unsigned long now = millis();
  if (now - last_render_time > ANIMATION_REFRESH)
  {
    process_all_windows();
    draw();
    last_render_time = now;
  }
}