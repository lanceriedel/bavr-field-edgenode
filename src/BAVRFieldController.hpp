#ifndef BAVRFieldController_h
#define BAVRFieldController_h
#include "config.hpp"
#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LaserDetect.hpp"
#include "TroughDetect.hpp"
#include "BallDetect.hpp"
#include <ArduinoJson.h>

class BAVRFieldController
{
public:
    BAVRFieldController(LEDAnimations* led_animations, 
    LaserDetect* laser_detect, 
    BAVRFieldComms* field_comms,
     TroughDetect* trough_detect, 
     BallDetect* ball_detect);
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
    void set_heater_pin(uint8_t p);
    void heater_on();
    void heater_off();

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
    uint8_t heater_pin;

    uint16_t current_fire_score = 0;
    uint16_t building_name_index;

//sensor types - in order are 
//LASER, TRENCH, BALL
const int config_types [NUM_BUILDINGS] [NUM_SENSORS_TYPES] { //initialize to zero
  {NO, NO, YES},          //RBO
  {YES, NO, NO},          //RTO
  {YES, YES, NO},         //RBM
  {YES, YES, NO},         //RTM
  {NO, YES, YES},         //RBI
  {NO, YES, YES},         //RTI
  {NO, YES, YES},         //LBI
  {NO, YES, YES},         //LTI
  {YES, YES, NO},         //LBM
  {YES, YES, NO},         //LTM
  {NO, NO, YES},         //LBO
  {YES, NO, NO}          //LTO
};

};

#endif



