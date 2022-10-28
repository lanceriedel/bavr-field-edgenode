#ifndef config_h
#define config_h
#include <pins_arduino.h>
//config.h

//heater
#define HEATER_PIN A2

#define RBO  0
#define RTO  1
#define RBM  2
#define RTM  3
#define RBI  4
#define RTI  5

#define LBI  6
#define LTI  7
#define LBM  8
#define LTM  9
#define LBO  10
#define LTO  11

#define LASER 0
#define TRENCH 1
#define BALL 2



#define YES 1
#define NO 0


#define NUM_BUILDINGS 12
#define NUM_SENSORS_TYPES 3

//sensor types - in order are 
//LASER, TRENCH, BALL
const int config [NUM_BUILDINGS] [NUM_SENSORS_TYPES] { //initialize to zero
  {NO, NO, YES},       //RBO
  {YES, NO, NO},         //RTO
  {YES, YES, NO},         //RBM
  {YES, YES, NO},         //RTM
  {NO, YES, YES},         //RBI
  {NO, YES, YES},         //RTI
  {YES, YES, NO},         //LBI
  {YES, YES, NO},         //LTI
  {YES, YES, NO},         //LBM
  {YES, YES, NO},         //LTM
  {NO, YES, YES},         //LBO
  {NO, YES, YES}          //LTO
};




#endif