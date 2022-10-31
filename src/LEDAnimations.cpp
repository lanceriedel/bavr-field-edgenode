#include "LEDAnimations.hpp"

uint32_t crgb_to_hex(CRGB color)
{
  return ((long)color.r << 16L) | ((long)color.g << 8L) | (long)color.b;
}

Gutter::Gutter()
{

}

Gutter::Gutter(uint16_t the_first_pixel)
{
  first_pixel = the_first_pixel;
}

void Gutter::setup()
{
  blackout_gutter();
}

void Gutter::blackout_gutter()
{
  set_progress(8,CRGB::Black);
}

void Gutter::set_progress(uint8_t steps, CRGB color)
{
  uint8_t num_segments = sizeof(segments_arr) / sizeof(segments_arr[0]);
  for (int i=0; i<num_segments; i++)
  {
    if (i < steps)
    {
      set_segment(i, color);
    }
    else
    {
      set_segment(i, CRGB::Black);
    }
  }
}

void Gutter::set_segment(uint8_t segment, CRGB color)
{
  set_mode(segments);
  segments_arr[segment] = color;
}

void Gutter::process_segments()
{
    uint8_t num_segments = sizeof(segments_arr) / sizeof(segments_arr[0]);
    uint8_t index = 0;

    for (int j=0; j < num_segments; j++) //for each segment
    {
      // do the indicating LEDs
      for (int i = 0; i<indicating; i++)
      {
        pixels[index] = segments_arr[j];
        index++;
      }
      //do the spacing LEDs
      for (int i=0; i<spacing; i++)
      {
        pixels[index] = CRGB::Black;
        index++;
      }
    }
}

void Gutter::set_notify(CRGB color)
{
  set_mode(notify);
  notify_color = color;
}
void Gutter::process_notify()
{
  for (int i=0; i<LEDS_PER_GUTTER; i++)
  {
    pixels[i] = notify_color;
  }
}

void Gutter::set_mode(op_mode new_mode)
{
  mode = new_mode;
}
void Gutter::compute()
{
  if (mode == segments)
  {
    process_segments();
  }
  else if (mode == notify)
  {
    process_notify();
  }
}

void Gutter::cp_data(CRGB *buffer)
{
  int k = first_pixel;
  for (int i = 0; i < LEDS_PER_GUTTER; i++)
  {
    buffer[k] = pixels[i]; //copy in the pixel data to the buffer
    k++;
  }
}



Laser::Laser()
{

}

Laser::Laser(uint16_t the_first_pixel)
{
  first_pixel = the_first_pixel;
}

void Laser::setup()
{
  mode = turnoff;
  blackout_laser();
}

void Laser::blackout_laser()
{

  for (int i=0; i<LEDS_PER_LASER; i++)
  {
    pixels[i] = CRGB::Black;
  }
}

void Laser::turnon_laser()
{
  for (int i=0; i<LEDS_PER_LASER; i++)
  {
    pixels[i] = CRGB::White;
  }
}

void Laser::set_mode(op_mode new_mode)
{
  mode = new_mode;
  last_laser_time = millis();
}

void Laser::compute()
{
  // unsigned long now = millis();
  // if (last_laser_time==0) {
  //   last_laser_time = millis();
  // }
  // if ((mode == turnon) && (now - last_laser_time > LASER_REFRESH))
  // {
  //   mode = turnoff;
  //   Serial.print("-");
  // }

  if (mode == turnon)
  {
    turnon_laser();
   // Serial.print("+");
    last_laser_time = millis();
  }
  if (mode == turnoff)
  {
   // Serial.print("|");
    blackout_laser();
  }
}

void Laser::cp_data(CRGB *buffer)
{
  int k = first_pixel;
  for (int i = 0; i < LEDS_PER_LASER; i++)
  {
    buffer[k] = pixels[i]; //copy in the pixel data to the buffer
    k++;
  }
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
  for (int i = 0; i < STRANDS_PER_WINDOW; i++)
  {
    for (int j = 0; j < LEDS_PER_STRAND; j++)
    {
      pixels[i][j] = CRGB::Black;
    }
  }
}

void Window::setup()
{
  blackout_window();
}


void Window::damaged()
{
  for (int i = 0; i < STRANDS_PER_WINDOW; i++)
  {
    for (int j = 0; j < LEDS_PER_STRAND; j++)
    {
      CRGB color = CRGB(0xFF8800);

      int pixelnumber;
      pixels[i][j] = color; //fill a strand at a time
    }
  }
}

void Window::fake_fire()
// set arbitrary height, copy in num flames...
{
  for (int i = 0; i < STRANDS_PER_WINDOW; i++)
  {
    bool reverse = false; //this is the reversing function to handle strips that are oriented 180 from each other
    if (i % 2 != 0)
    {
      reverse = true;
    }
    // calculate a random height for the flame for this strand
    uint8_t height = random(LEDS_PER_STRAND / 4, LEDS_PER_STRAND);
    for (int j = 0; j < LEDS_PER_STRAND; j++)
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
        pixelnumber = (LEDS_PER_STRAND - 1) - j;
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
  if (is_damaged) {
    damaged();
  }
  else if (on_fire)
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
  for (int i = 0; i < STRANDS_PER_WINDOW; i++) //for each strand of the window
  {
    for (int j = 0; j < LEDS_PER_STRAND; j++) //for each led in the strand
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


void Building::set_damaged_windows(uint8_t side, uint8_t windows)
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
        sides[side].windows[0].is_damaged = true;
        sides[side].windows[1].is_damaged = true;
      }
      else if (windows == 1)
      {
        sides[side].windows[0].is_damaged = true;
        sides[side].windows[1].is_damaged = false;
      }
      else
      {
        sides[side].windows[0].is_damaged = false;
        sides[side].windows[1].is_damaged = false;
      }
    }
  }
}


void Building::set_active_laser(uint8_t side) {
  Serial.print("Setting active laser !!!! : ");Serial.println(side);

  sides[side].laser.set_mode(Laser::turnon);
}

void Building::set_inactive_laser(uint8_t side) {
  Serial.print("Setting inactive laser in!!!! : ");Serial.println(side);

  sides[side].laser.set_mode(Laser::turnoff);
}

void Building::set_gutter_progress(uint8_t progress, CRGB color)
{
  uint8_t num_sides = (sizeof(sides) / sizeof(sides[0]));
  for (int i=0; i<num_sides; i++)
  {
    sides[i].gutter.set_progress(progress, color);
  }
}

void Building::set_gutter_segment(uint8_t segment, CRGB color)
{
  uint8_t num_sides = (sizeof(sides) / sizeof(sides[0]));
  for (int i=0; i<num_sides; i++)
  {
    sides[i].gutter.set_segment(segment, color);
  }
}
void Building::set_gutter_full(CRGB color)
{
  uint8_t num_sides = (sizeof(sides) / sizeof(sides[0]));
  for (int i=0; i<num_sides; i++)
  {
    sides[i].gutter.set_notify(color);
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
  building.sides[0].gutter = Gutter(0);
  building.sides[1].gutter = Gutter(1 * LEDS_PER_GUTTER);
  building.sides[2].gutter = Gutter(2 * LEDS_PER_GUTTER);
  building.sides[3].gutter = Gutter(3 * LEDS_PER_GUTTER); //this is an awful way to stack the strip but..it works?
  building.sides[0].laser = Laser(0);
  building.sides[1].laser = Laser(0);
  building.sides[2].laser = Laser(0);
  building.sides[3].laser = Laser(0);

}

void LEDAnimations::draw()
{
  // NOT sure yet
  FastLED.show();
}

void LEDAnimations::setup()
{
  FastLED.addLeds<WS2812, DATA_PIN_WINDOWS, GRB>(windows[0], 8 * STRANDS_PER_WINDOW * LEDS_PER_STRAND);
  FastLED.addLeds<WS2812, DATA_PIN_GUTTERS, GRB>(gutters[0], 4 * LEDS_PER_GUTTER);
  FastLED.addLeds<WS2812, DATA_PIN_LASER_0, GRB>(lasers[0], LEDS_PER_LASER);
  FastLED.addLeds<WS2812, DATA_PIN_LASER_1, GRB>(lasers[1], LEDS_PER_LASER);
  FastLED.addLeds<WS2812, DATA_PIN_LASER_2, GRB>(lasers[2], LEDS_PER_LASER);
  FastLED.addLeds<WS2812, DATA_PIN_LASER_3, GRB>(lasers[3], LEDS_PER_LASER);

}

void LEDAnimations::process_window(uint8_t side, uint8_t window)
{
  building.sides[side].windows[window].compute();
  building.sides[side].windows[window].cp_data(windows[0]);
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

void LEDAnimations::process_laser(uint8_t side)
{
  building.sides[side].laser.compute();
  //Serial.print(" process laser for[");Serial.println(side);

  building.sides[side].laser.cp_data(lasers[side]);
}

void LEDAnimations::process_all_lasers()
{
  int lenSides = 4 ;
  //Serial.println("");
  for (int i = 0; i < lenSides; i++)
  {
    process_laser(i);
  }
 // Serial.println("");
}

void LEDAnimations::process_gutter(uint8_t side)
{
  building.sides[side].gutter.compute();
  building.sides[side].gutter.cp_data(gutters[0]);
}
void LEDAnimations::process_all_gutters()
{
  uint8_t lenSides = sizeof(building.sides) / sizeof(building.sides[0]);
  for (int i = 0; i < lenSides; i++)
  {
    process_gutter(i);
  }
}

void LEDAnimations::loop()
{
  unsigned long now = millis();
  if (now - last_render_time > ANIMATION_REFRESH)
  {
    process_all_windows();
    process_all_gutters();
    process_all_lasers();
    draw();
    last_render_time = now;
  }
}

void LEDAnimations::boot_sequence(uint8_t progress)
{
  building.set_gutter_progress(progress, CRGB::Blue);
  process_all_gutters();
  draw();
}