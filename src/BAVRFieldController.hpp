#ifndef BAVRFieldController_h
#define BAVRFieldController_h

#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LaserDetect.hpp"
#include "TroughDetect.hpp"
#include "BallDetect.hpp"

class BAVRFieldController
{
public:
    BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms, TroughDetect* trough_detect, BallDetect* ball_detect);
    boolean setup(const char* unique_id);
    void loop();
    void callback(char* topic, byte* payload, unsigned int length);
    void event_trigger(const char* event, int whichone);
    void laser_hit_message(int hits, int whichone);
    void ball_detect_message(int drops);
    void interrupt(int pin);

    void subscribe_all();



private:
    BAVRFieldComms* field_comms;
    LEDAnimations* led_animations;
    LaserDetect* laser_detect;
    TroughDetect* trough_detect;
    BallDetect* ball_detect;
    char node_id[128];

    uint16_t current_fire_score = 0;
};

#endif



