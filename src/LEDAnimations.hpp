#ifndef LEDAnimations_h
#define LEDAnimations_h

#include <FastLED.h>

#define DATA_PIN_WINDOWS A0
#define DATA_PIN_GUTTERS A5

#define STRANDS_PER_WINDOW 3
#define LEDS_PER_STRAND 8

#define LEDS_PER_GUTTER 24

#define ANIMATION_REFRESH 250

const CRGB flames_colors[8] = {
    // descending heat levels (yellow - hottest), (red-coolest)
    CRGB(0xFF8800), // yellow
    CRGB(0xFF7800),
    CRGB(0xFF6800),
    CRGB(0xFF5C05), // orange
    CRGB(0xFF4100),
    CRGB(0xFF3800), // red-orange
    CRGB(0xFF5607),
    CRGB(0xFF0000), // red
};

uint32_t crgb_to_hex(CRGB color); // fn to convert CRGB for printing

class Gutter
{

public:
    Gutter();
    Gutter(uint16_t the_first_pixel);
    uint8_t indicating = 2;
    uint8_t spacing = 1;
    CRGB notify_color = CRGB::Blue;

    enum op_mode 
    {
        segments,
        notify
    };

    void setup();
    void blackout_gutter();
    void cp_data(CRGB *buffer);

    void set_progress(uint8_t steps);
    void set_segment(uint8_t segment, bool enable);
    void lightup_gutter(CRGB color);

    void set_mode(op_mode mode);
    void compute();
    void process_segments();
    void process_notify();

private:
    CRGB pixels[LEDS_PER_GUTTER];
    uint16_t first_pixel;//index of this objects first pixel in the total pixel array
    op_mode mode=segments;
    bool segments_arr[8] = {false};

};

class Window
{
private:
    CRGB pixels[STRANDS_PER_WINDOW][LEDS_PER_STRAND];
    uint16_t first_pixel;//index of this objects first pixel in the total pixel array

public:
    Window();
    Window(uint16_t the_first_pixel);

    bool on_fire = false;

    void setup();
    void blackout_window(); //remove the fire
    void fake_fire(); //fill the pixel array with fire
    void cp_data(CRGB *buffer); // the buffer that contains the pixels where this window resides
    void compute(); //recompute the pixels for the window based on 'on_fire' or not
};

class Side
{
public:
    Side();
    Window windows[2];
    Gutter gutter;
};

class Building
{
public:
    Building();
    Side sides[4];
    void set_active_windows(uint8_t side, uint8_t windows); //set which windows are on fire or not
    void set_gutter_progress(uint8_t progress); //sets the progress for all gutters
    //void set_gutter_segments(uint8_t segment, bool enable);
};

class LEDAnimations
{
private:
    CRGB windows[1][8 * STRANDS_PER_WINDOW * LEDS_PER_STRAND];
    CRGB gutters[1][4 * LEDS_PER_GUTTER];
    unsigned long last_render_time = 0;

public:
    LEDAnimations();

    Building building;

    void setup();
    void process_window(uint8_t side, uint8_t window); // recompute window and copy pixels to buffer
    void process_all_windows(); //do this for all available windows
    void process_gutter(uint8_t side);// do this for a single gutter
    void process_all_gutters(); //do this for all gutters
    void loop(); //main loop for this task
    void draw(); //write the pixels out to the strip
    void boot_sequence(uint8_t progress);
};

#endif