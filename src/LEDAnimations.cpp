#include "LEDAnimations.hpp"

LEDStrip::LEDStrip()
{}

LEDStrip::LEDStrip(uint8_t pin)
{

}

void LEDStrip::set_pixel_color(uint8_t pixel_index, CRGB color)
{
    if ( (0 <= pixel_index) && (pixel_index < length()))
    {
        pixels[pixel_index] = color;
    }
}

void LEDStrip::set_strip_color(CRGB color)
{
    for (int i=0; i < length(); i++)
    {
        set_pixel_color(i, color);
    }
}

void LEDStrip::set_progress(uint8_t num_pixels, CRGB color)
{
    if (num_pixels < length())
    {
        for (int i=0; i < length(); i++)
        {
            if (i < num_pixels)
            {
                set_pixel_color(i, color);
            }
            else
            {
                set_pixel_color(i, CRGB::Black);
            }
        }
    }
}

void LEDStrip::blackout_strip()
{
    for (int i=0; i < length(); i++)
    {
        set_pixel_color(i, CRGB::Black);
    }
}

uint8_t LEDStrip::length()
{
    return sizeof(pixels)/sizeof(pixels[0]);
}

LED::LED()
{

}

LED::LED(uint8_t pin)
{
    this->pin = pin;
}

void LED::turn_led_on()
{
    state=true;
}

void LED::turn_led_off()
{
    state=false;
}

void LED::set_led_state(bool state)
{
    this->state = state;
}

uint8_t LED::get_val()
{
    if (state) return 1;
    else return 0;
}

LEDAnimations::LEDAnimations()
{

}

void LEDAnimations::setup()
{
    leds[0] = LED(LED_PIN_MOS0);
    leds[1] = LED(LED_PIN_MOS1);
    leds[2] = LED(LED_PIN_MOS2);

    pinMode(LED_PIN_MOS0,OUTPUT);
    pinMode(LED_PIN_MOS1,OUTPUT);
    pinMode(LED_PIN_MOS2,OUTPUT);

    pinMode(LED_PIN_STRIP0,OUTPUT);
    pinMode(LED_PIN_STRIP1,OUTPUT);
    pinMode(LED_PIN_STRIP2,OUTPUT);
    pinMode(LED_PIN_STRIP3,OUTPUT);

    strips[0] = LEDStrip();
    FastLED.addLeds<WS2812, LED_PIN_STRIP0, RGB>(&strips[0].pixels[0], strips[0].length());
    strips[1] = LEDStrip();
    FastLED.addLeds<WS2812, LED_PIN_STRIP1, RGB>(&strips[1].pixels[0], strips[1].length());
    strips[2] = LEDStrip();
    FastLED.addLeds<WS2812, LED_PIN_STRIP2, RGB>(&strips[2].pixels[0], strips[2].length());
    strips[3] = LEDStrip();
    FastLED.addLeds<WS2812, LED_PIN_STRIP3, RGB>(&strips[3].pixels[0], strips[3].length());

}

void LEDAnimations::loop()
{
  unsigned long now = millis();
  if (now - last_render_time > ANIMATION_REFRESH)
  {
    draw();
    last_render_time = now;
  }
}

void LEDAnimations::draw()
{
    FastLED.show();
    uint8_t len_leds = sizeof(leds)/sizeof(leds[0]);
    for (int i=0; i<len_leds; i++)
    {
        digitalWrite(leds[i].pin, leds[i].get_val());
    }
}

void LEDAnimations::boot_sequence(uint8_t progress)
{
    uint8_t length_strips = sizeof(strips)/sizeof(strips[0]);
    for (int i=0; i<length_strips; i++)
    {
        strips[i].set_progress(progress*3, CRGB::Blue);
    }
    draw();
}