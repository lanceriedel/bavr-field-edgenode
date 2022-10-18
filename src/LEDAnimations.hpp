#ifndef LEDAnimations_h
#define LEDAnimations_h


#include <FastLED.h>



#define DATA_PIN_WALL_1 A0
#define TOTAL_LEDS 300

#define STRANDS_PER_WINDOW 3
#define LEDS_PER_STRAND 8

const CRGB flames_colors[8] = { //descending heat levels (yellow - hottest), (red-coolest)
    CRGB(0xFF8800), //yellow
    CRGB(0xFF7800),
    CRGB(0xFF6800),
    CRGB(0xFF5C05), //orange
    CRGB(0xFF4100),
    CRGB(0xFF3800), //red-orange
    CRGB(0xFF5607),
    CRGB(0xFF0000), //red
};

class Window
{
private:
    uint8_t strands = STRANDS_PER_WINDOW;
    uint8_t leds_per_strand = LEDS_PER_STRAND;
    CRGB pixels [STRANDS_PER_WINDOW][LEDS_PER_STRAND];
public:
    Window();
    void setup();
    void blackout_window();
    void fake_fire();
    void calculate_fire();
    uint32_t crgb_to_hex(CRGB color);
    bool on_fire = false;

    void cp_data(CRGB* buffer, uint16_t offset); //give the STARTING index of the main buffer for the window
};

class LEDAnimations
{
public:
    LEDAnimations();
    void setup();
    void process_window(uint8_t index);
    void process_all_windows();
    void loop();
    void draw();

    Window windows[8];

private:
    CRGB entire_thing[1][TOTAL_LEDS];
    unsigned long last_render_time = 0;
};



#endif
