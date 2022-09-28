#include "BallDetect.hpp"
#include <Wire.h>

BallDetect::BallDetect()
{
}

void BallDetect::ball_init() {
  
}

//Too keep things clean, we are going to check this on the controller loop and clear
bool BallDetect::triggered() {
   if (hastriggered) {
    hastriggered = false;
    return true;
   } else
   return false;
}


void BallDetect::ball_trigger() {
    hastriggered = true;
    //client->publish("avr-building","ball-dropped");
}

bool BallDetect::ball_detect() {
  
  if (digitalRead(BALL_COUNTER_PIN)==TRIGGER_STATE) {
    ball_trigger();
    return true;
  }
  return false;

}

