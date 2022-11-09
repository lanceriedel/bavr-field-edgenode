#ifndef config_h
#define config_h
#include <pins_arduino.h>
//config.h

//heater
#define HEATER_PIN A2
#define BALL_DROP_PIN 2

#define RBO  ((uint8_t)0)
#define RTO  ((uint8_t)1)
#define RBM  ((uint8_t)2)
#define RTM  ((uint8_t)3)
#define RBI  ((uint8_t)4)
#define RTI  ((uint8_t)5)

#define LBI  ((uint8_t)6)
#define LTI  ((uint8_t)7)
#define LBM  ((uint8_t)8)
#define LTM  ((uint8_t)9)
#define LBO  ((uint8_t)10)
#define LTO  ((uint8_t)11)

#define UNDEFINED_BLDG ((uint8_t)32)

#define LASER ((uint8_t)0)
#define TRENCH ((uint8_t)1)
#define BALL ((uint8_t)2)

#define YES ((uint8_t)1)
#define NO ((uint8_t)0)

#define NUM_BUILDINGS 12
#define NUM_SENSORS_TYPES 3

//mqtt
static const int mqtt_server[] = {192,168,1,112};
static const int mqtt_port = 1883;

//lights
#define ANIMATION_REFRESH 100
#define LED_PIN_STRIP1 A5
#define LED_PIN_STRIP2 A0
#define LENGTH_STRIPS 30

#define LED_PIN_MOS1 A1
#define LED_PIN_MOS2 A2
#define LED_PIN_MOS3 A3

#endif