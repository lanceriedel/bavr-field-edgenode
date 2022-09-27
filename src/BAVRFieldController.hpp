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
    boolean setup(String unique_id);
    void loop();
    void callback(char* topic, byte* payload, unsigned int length);
    void event_trigger(String event);



private:
    BAVRFieldComms* field_comms;
    LEDAnimations* led_animations;
    LaserDetect* laser_detect;
    TroughDetect* trough_detect;
};

#endif



