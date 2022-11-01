#include "LEDAnimationsOther.hpp"

uint32_t LEDAnimationsOther::crgb_to_hex(CRGB color)
{
  return ((long)color.r << 16L) | ((long)color.g << 8L) | (long)color.b;
}



Path::Path()
{

}

Path::Path(uint16_t the_first_pixel)
{
  first_pixel = the_first_pixel;
}

void Path::setup()
{
  mode = turnoff;
  blackout_path();
}

void Path::blackout_path()
{

  for (int i=0; i<LEDS_PER_PATH; i++)
  {
    pixels[i] = CRGB::Black;
  }
}

void Path::turnon_path()
{
  for (int i=0; i<LEDS_PER_PATH; i++)
  {
    pixels[i] = color;
  }
}

void Path::set_mode(op_mode new_mode)
{
  mode = new_mode;
}

void Path::set_color(CRGB new_color)
{
  color = new_color;
}

void Path::compute()
{
 
  if (mode == turnon)
  {
    turnon_path();
    Serial.print("P+");
  }
  if (mode == turnoff)
  {
   Serial.print("P|");
    blackout_path();
  }
}

void Path::cp_data(CRGB *buffer)
{
  int k = first_pixel;
  for (int i = 0; i < LEDS_PER_PATH; i++)
  {
    buffer[k] = pixels[i]; //copy in the pixel data to the buffer
    k++;
  }
}


Trench::Trench()
{

}

Trench::Trench(uint16_t the_first_pixel)
{
  first_pixel = the_first_pixel;
}

void Trench::setup()
{
  mode = turnoff;
  blackout_trench();
}

void Trench::blackout_trench()
{

  for (int i=0; i<LEDS_PER_TRENCH; i++)
  {
    pixels[i] = CRGB::Black;
  }
}

void Trench::turnon_trench()
{
  for (int i=0; i<LEDS_PER_TRENCH; i++)
  {
    pixels[i] = color;
  }
}

void Trench::set_mode(op_mode new_mode)
{
  mode = new_mode;
}

void Trench::set_color(CRGB new_color)
{
  color = new_color;
}

void Trench::compute()
{

  if (mode == turnon)
  {
    turnon_trench();
   Serial.print("t+");
  }
  if (mode == turnoff)
  {
   Serial.print("t|");
    blackout_trench();
  }
}

void Trench::cp_data(CRGB *buffer)
{
  int k = first_pixel;
  for (int i = 0; i < LEDS_PER_TRENCH; i++)
  {
    buffer[k] = pixels[i]; //copy in the pixel data to the buffer
    k++;
  }
}


LEDAnimationsOther::LEDAnimationsOther()
{
  trench[0] = Trench(0);
  trench[1] = Trench(0);

  path[0] = Path(0);
  path[1] = Path(0);

}

void LEDAnimationsOther::draw()
{
  // NOT sure yet
  FastLED.show();
  Serial.println("draw everything");
}

void LEDAnimationsOther::setup()
{

  pinMode(DATA_PIN_FIREPATH_0,OUTPUT);
  pinMode(DATA_PIN_FIREPATH_1,OUTPUT);
  pinMode(DATA_PIN_TRENCH_0,OUTPUT);
  pinMode(DATA_PIN_TRENCH_1,OUTPUT);

  FastLED.addLeds<WS2812, DATA_PIN_FIREPATH_0, GRB>(paths[0], LEDS_PER_PATH);
  FastLED.addLeds<WS2812, DATA_PIN_FIREPATH_1, GRB>(paths[1], LEDS_PER_PATH);

  FastLED.addLeds<WS2812, DATA_PIN_TRENCH_0, GRB>(trenches[0], LEDS_PER_TRENCH);
  FastLED.addLeds<WS2812, DATA_PIN_TRENCH_1, GRB>(trenches[1], LEDS_PER_TRENCH);
  path[0].blackout_path();
  path[1].blackout_path();
  trench[0].blackout_trench();
  trench[1].blackout_trench();

}



void LEDAnimationsOther::process_path(uint8_t side)
{
  path[side].compute();
  path[side].cp_data(paths[side]);
}

void LEDAnimationsOther::process_trench(uint8_t side)
{
  trench[side].compute();
  trench[side].cp_data(trenches[side]);
}

void LEDAnimationsOther::process_all_trenches()
{
  int lenSides = 2 ;

  for (int i = 0; i < lenSides; i++)
  {
    process_trench(i);
  }

}

void LEDAnimationsOther::process_all_paths()
{
  int lenSides = 2 ;
 
  for (int i = 0; i < lenSides; i++)
  {
    process_path(i);
  }

}

void LEDAnimationsOther::set_active_trench(uint8_t side, CRGB color) {
  Serial.print("Setting active trench !!!! : ");Serial.println(side);

  trench[side].set_mode(Trench::turnon);
  trench[side].set_color(color);
}

void LEDAnimationsOther::set_inactive_trench(uint8_t side) {
  Serial.print("Setting inactive trench in!!!! : ");Serial.println(side);

  trench[side].set_mode(Trench::turnoff);
}

void LEDAnimationsOther::set_active_path(uint8_t side, CRGB color) {
  Serial.print("Setting active path !!!! : ");Serial.println(side);

  path[side].set_mode(Path::turnon);
  path[side].set_color(color);

}

void LEDAnimationsOther::set_inactive_path(uint8_t side) {
  Serial.print("Setting inactive path in!!!! : ");Serial.println(side);

  path[side].set_mode(Path::turnoff);
}



void LEDAnimationsOther::loop()
{
  unsigned long now = millis();
       // Serial.print("--oop other animatins: ");Serial.println(last_render_time);

  if (now - last_render_time > ANIMATION_REFRESH)
  {
    process_all_trenches();
    process_all_paths();
    
    draw();
    last_render_time = now;
      Serial.print("loop other animatins: ");Serial.println(last_render_time);

  }
}
