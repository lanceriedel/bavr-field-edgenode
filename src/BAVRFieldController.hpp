#ifndef BAVRFieldController_h
#define BAVRFieldController_h

#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LaserDetect.hpp"
#include "TroughDetect.hpp"
#include "BallDetect.hpp"
#include <ArduinoJson.h>

class BAVRFieldController
{
public:
    BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms, TroughDetect* trough_detect, BallDetect* ball_detect);
    boolean setup(const char* unique_id);
    void loop();
    void callback(char* topic, byte* payload, unsigned int length);
    void reset_match();
    void event_trigger(const char* event, int whichone);
    void laser_hit_message(int hits, int whichone);
    void ball_detect_message(int drops);
    void trough_detect_message(int bags);
    void laser_last_raw_reading_message();
    void last_weight_tare_reading_message();

    void interrupt(int pin);

    void subscribe_all();
    void reset_all();
    void clean_buffers();



private:
    void set_config();
    BAVRFieldComms* field_comms;
    LEDAnimations* led_animations;
    LaserDetect* laser_detect;
    TroughDetect* trough_detect;
    BallDetect* ball_detect;
    char node_id[128];
    StaticJsonDocument<512> json;
    char topic[256];
    char message[512];
    char uuid[32];

    uint16_t current_fire_score = 0;
};

#endif



