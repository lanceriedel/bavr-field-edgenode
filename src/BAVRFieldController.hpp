#ifndef BAVRFieldController_h
#define BAVRFieldController_h
#include "config.hpp"
#include "BAVRFieldComms.hpp"
#include "LEDAnimations.hpp"
#include "LaserDetect.hpp"
#include "BallDetect.hpp"
#include <ArduinoJson.h>

class BAVRFieldController
{
public:
    BAVRFieldController(LEDAnimations* led_animations,
    LaserDetect* laser_detect,
    BAVRFieldComms* field_comms,
    BallDetect* ball_detect);

    int worst_memory = 9999;

    void check_memory();

    boolean setup(const char* unique_id);
    void loop();
    void callback(char* topic, byte* payload, unsigned int length);
    void reset_match();
    void event_trigger(const char* event);
    void laser_hit_message(int hits);
    void ball_detect_message(int drops);
    void laser_last_raw_reading_message();
    void heartbeat_message();

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
    BallDetect* ball_detect;
    char node_id[128];
    StaticJsonDocument<512> json;
    char topic[256];
    char message[1024];
    char uuid[32];
    uint8_t heater_pin;
    bool isheater_on =false;

    uint16_t current_fire_score = 0;
    uint8_t building_name_index;
    uint32_t last_laser_time;
    uint32_t last_heartbeat_time =0;
    const uint32_t MAX_HEARTBEAT_WAIT =10000;
    const uint32_t MAX_LASER_INDICATOR = 200;
    uint8_t lastone = 0;
    uint8_t thisone = 0;
    bool configured = false;
//sensor types - in order are 
//LASER, TRENCH, BALL
const uint8_t config_types [NUM_BUILDINGS] [NUM_SENSORS_TYPES] { //initialize to zero
  {NO, NO, YES},          //RBO
  {YES, NO, NO},          //RTO
  {YES, NO, NO},         //RBM
  {YES, NO, NO},         //RTM
  {NO, NO, YES},         //RBI
  {NO, NO, YES},         //RTI
  {NO, NO, YES},         //LBI
  {NO, NO, YES},         //LTI
  {YES, NO, NO},         //LBM
  {YES, NO, NO},         //LTM
  {NO, NO, YES},         //LBO
  {YES, NO, NO},          //LTO
  {NO, NO, NO},          //LTT
  {NO, NO, NO},          //RTT
};

};

#endif



