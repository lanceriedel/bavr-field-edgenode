#include "BallDetect.hpp"

BallDetect::BallDetect()
{
}

void BallDetect::ball_init(int pin) {
  this->pin = pin;
  pinMode(pin, INPUT_PULLUP);
  digitalWrite(pin, HIGH); // turn on the pullup
}

int BallDetect::get_pin() {
  return this->pin ;
}

void BallDetect::ball_trigger() {
    Serial.println(F("Ball Triggered true"));
    if (millis()-lasttrigger > MAX_WAIT) {
      Serial.println(F("XXXXXXXXXXXXXXXX BALL DROP X"));

      lasttrigger = millis();
      hastriggered = true;
      sensorState = 1;
      lasttrigger = millis();
    } 
 
    //client->publish("avr-building","ball-dropped");
}

bool BallDetect::ball_detect() {
  sensorState = digitalRead(pin);

   if (sensorState && !lastState) {
     lastState = sensorState;
      hastriggered = false;
     Serial.println("Unbroken");
   } 
   if (!sensorState && lastState) {
     Serial.println("Broken");
     //ball_trigger();
     lastState = sensorState;
    // return true;

   }  
  if (hastriggered == true) {
      hastriggered = false;
      Serial.println(F("Check ball detect true"));
      return true;
  }
  else

  // if (sensorState==TRIGGER_STATE) {
  //  // ball_trigger();
  //   return true;
  // }
    return false;

}

