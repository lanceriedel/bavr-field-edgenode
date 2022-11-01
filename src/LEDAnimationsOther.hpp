#ifndef LEDAnimationsOther_h
#define LEDAnimationsOther_h

#include <FastLED.h>


#define DATA_PIN_TRENCH_0 A1
#define DATA_PIN_TRENCH_1  A3
#define DATA_PIN_FIREPATH_0 5
#define DATA_PIN_FIREPATH_1 A4

#define LEDS_PER_PATH 24

#define LEDS_PER_TRENCH 24

#define ANIMATION_REFRESH 250


uint32_t crgb_to_hex(CRGB color); // fn to convert CRGB for printing


class Trench
{

public:
    Trench();
    Trench(uint16_t the_first_pixel);

    enum op_mode
    {
        turnon,
        turnoff
    };

    void setup();
    void blackout_trench();
    void turnon_trench();
    void cp_data(CRGB *buffer);


    void set_mode(op_mode new_mode);
    void set_color(CRGB new_color);

    void compute();
    CRGB color;

private:
    CRGB pixels[LEDS_PER_TRENCH];
    uint16_t first_pixel;//index of this objects first pixel in the total pixel array
    op_mode mode=turnoff;
};

class Path
{

public:
    Path();
    Path(uint16_t the_first_pixel);

    enum op_mode
    {
        turnon,
        turnoff
    };

    void setup();
    void blackout_path();
    void turnon_path();
    void cp_data(CRGB *buffer);


    void set_mode(op_mode new_mode);
    void set_color(CRGB new_color);
    void compute();

private:
    CRGB pixels[LEDS_PER_PATH];
    CRGB color;
    uint16_t first_pixel;//index of this objects first pixel in the total pixel array
    op_mode mode=turnoff;
};

class LEDAnimationsOther
{
private:
    CRGB trenches[2][1 * LEDS_PER_TRENCH];
    CRGB paths[2][1 * LEDS_PER_PATH];

    unsigned long last_render_time = 0;

public:
    LEDAnimationsOther();
    uint32_t crgb_to_hex(CRGB color);

    Trench trench[2];
    Path path[2];


    void setup();

    void process_trench(uint8_t side);// do this for all trenches
    void process_path(uint8_t side);// do this for all paths

    void process_all_paths();
    void process_all_trenches();

    void set_active_trench(uint8_t side, CRGB color);
    void set_inactive_trench(uint8_t side);

    void set_active_path(uint8_t side, CRGB color);
    void set_inactive_path(uint8_t side);

    void loop(); //main loop for this task
    void draw(); //write the pixels out to the strip
};

#endif