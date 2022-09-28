#ifndef BAVRFieldController_h
#define BAVRFieldController_h

#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LaserDetect.hpp"
#include "TroughDetect.hpp"

class BAVRFieldController
{
public:
    BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms, TroughDetect* trough_detect);
    boolean setup(const char* unique_id);
    void loop();
    void callback(char* topic, byte* payload, unsigned int length);
    void event_trigger(const char* event);
    void laser_hit_message(int hit);



private:
    BAVRFieldComms* field_comms;
    LEDAnimations* led_animations;
    LaserDetect* laser_detect;
    TroughDetect* trough_detect;
    char node_id[128];
};

#endif



