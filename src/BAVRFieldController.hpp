#ifndef BAVRFieldController_h
#define BAVRFieldController_h

#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LaserDetect.hpp"

class BAVRFieldController
{
public:
    BAVRFieldController(LEDAnimations& led_animations, LaserDetect& laser_detect, BAVRFieldComms& filed_comms);
    boolean setup(String unique_id, PubSubClient& client);
    void loop();
    void callback(char* topic, byte* payload, unsigned int length);


private:
    BAVRFieldComms* filed_comms;
    LEDAnimations* led_animations;
    LaserDetect* laser_detect;
};

#endif



