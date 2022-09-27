#include "BAVRFieldController.hpp"

BAVRFieldController::BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms, TroughDetect* trough_detect)
{
    this->laser_detect = laser_detect;
    this->led_animations = led_animations;
    this->field_comms = field_comms;
    this->trough_detect = trough_detect;
}


//Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void BAVRFieldController::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print((topic));
  Serial.print(F("] "));
  String topicstr(topic);

  if (topicstr.startsWith("led")) {
    
  }
  if (topicstr.equals("windowon")) {
      led_animations->ledanimate(1);
      Serial.println("windowon msg");
  }

  if (topicstr.equals("windowoff")) {
      led_animations->ledanimate(0);
      
      Serial.println("windowoff msg");
  }
  
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

void BAVRFieldController::event_trigger(String event) {
    if (event.equals("laser")) {
      Serial.println(F("Controller: LASER event triggered"));
      field_comms->message("avr-building/1/laser-hit","1");
    }
    else if (event.equals("trough")) {
      Serial.println(F("Controller: Trough event triggered"));
      field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
    }
}

///Everybody do a loop   -- make sure no one is hogging the one thread please!!
void BAVRFieldController::loop() {
  
  field_comms->loop();
  boolean trigger = laser_detect->laser_detect();
  if (trigger) {
    event_trigger("laser");
  }
  led_animations->ledanimate();

  if(trough_detect->trough_detect())
    event_trigger("trough");

}

boolean BAVRFieldController::setup(String unique_id) {

}

