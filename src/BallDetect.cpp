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

void BallDetect::ball_trigger_interrupt() {
    Serial.println(F("Ball Triggered true"));
    if (millis()-lasttrigger > MAX_WAIT) {
      Serial.println(F("XXXXXXXXXXXXXXXX BALL DROP X"));
      hastriggered = true;
      sensorState = 1;
      lasttrigger = millis();
    } else  {
            Serial.println(F("------ FALSE READING BALL DROP X"));

    }
 
    //client->publish("avr-building","ball-dropped");
}


bool BallDetect::ball_detect() {
      Serial.println(F("Check ball detect...."));

  //Not needed if using trigger

  // sensorState = digitalRead(pin);

  // if (sensorState && !lastState) {
  //   lastState = sensorState;
  //   hastriggered = false;
  //   Serial.println("Unbroken");
  // }  else if (!sensorState && lastState) {
  //    Serial.println("Broken");
  // //    //ball_trigger();
  //    lastState = sensorState;
  //    // return true;

  // }  
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

