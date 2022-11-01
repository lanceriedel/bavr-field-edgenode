#ifndef BAVRFieldController_h
#define BAVRFieldController_h
#include "config.hpp"
#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LEDAnimationsOther.hpp"
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
    LEDAnimationsOther* led_animations_other;

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
    uint8_t building_name_index;
    uint32_t last_laser_time;

    const uint32_t MAX_LASER_INDICATOR = 750;
    uint8_t lastone = 0;
    boolean hasOtherLEDAnimations = false;
 
//sensor types - in order are 
//LASER, TRENCH, BALL
const uint8_t config_types [NUM_BUILDINGS] [NUM_SENSORS_TYPES] { //initialize to zero
  {NO, NO, YES, NO},          //RBO
  {YES, NO, NO, NO},          //RTO
  {YES, YES, NO, NO},         //RBM
  {YES, YES, NO, NO},         //RTM
  {NO, YES, YES, NO},         //RBI
  {NO, YES, YES, NO},         //RTI
  {NO, YES, YES, NO},         //LBI
  {NO, YES, YES, NO},         //LTI
  {YES, YES, NO, NO},         //LBM
  {YES, YES, NO, NO},         //LTM
  {NO, NO, YES, NO},         //LBO
  {YES, NO, NO, NO},          //LTO
  {NO, NO, NO, YES},         //LTT
  {NO, NO, NO, YES}          //RTT
};

};

#endif



