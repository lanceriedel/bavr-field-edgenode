#ifndef BAVRFieldController_h
#define BAVRFieldController_h

#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LaserDetect.hpp"

class BAVRFieldController
{
public:
    BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms);
    boolean setup(String unique_id);
    void loop();
    void callback(char* topic, byte* payload, unsigned int length);
    void event_trigger(String event);



private:
    BAVRFieldComms* field_comms;
    LEDAnimations* led_animations;
    LaserDetect* laser_detect;
};

#endif



