#ifndef LEDAnimations_h
#define LEDAnimations_h

#include "config.hpp"

#include <FastLED.h>

class LEDStrip
{
    public:
    LEDStrip();
    LEDStrip(uint8_t pin);

    CRGB pixels[LENGTH_STRIPS];

    void set_pixel_color(uint8_t pixel_index, CRGB color);
    void set_strip_color(CRGB color);
    void set_progress(uint8_t num_pixels, CRGB color);
    void blackout_strip();

    uint8_t length();
};

class LED
{
    public:
    LED();
    LED(uint8_t pin);
    void turn_led_on();
    void turn_led_off();
    void set_led_state(bool state);
    uint8_t get_val();
    bool state;
    uint8_t pin;
};

class LEDAnimations
{
private:
    unsigned long last_render_time = 0;

public:
    LEDAnimations();

    LED leds[3];
    LEDStrip strips[2];

    void setup();

    void loop(); //main loop for this task
    void draw(); //write the pixels out to the strip
    void boot_sequence(uint8_t progress);
};

#endif