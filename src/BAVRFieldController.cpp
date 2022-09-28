#include "BAVRFieldController.hpp"
//#include <ArduinoJson.h>


BAVRFieldController::BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms)
{
    this->laser_detect = laser_detect;
    this->led_animations = led_animations;
    this->field_comms = field_comms;
}


//Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void BAVRFieldController::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print((topic));
  Serial.print(F("] "));
  String topicstr(topic);
  char buffer[128];

  if (length>0) {
    strncpy(buffer, (const char*) payload, length);
    buffer[length] = 0;
  }

  if (topicstr.startsWith("led")) {
    
  }

  if (topicstr.startsWith("nodered/initialization")) {
        nodeID = String(buffer);
        Serial.print(F("Node id initialized: ")); Serial.println(nodeID);
  }



  if (topicstr.equals("windowon")) {
      led_animations->ledanimate(1);
      Serial.println("windowon msg");
  }

  if (topicstr.equals("windowoff")) {
      led_animations->ledanimate(0);
      
      Serial.println("windowoff msg");
  }
  //make this better
  
 Serial.println(buffer);
  Serial.println();
}

void BAVRFieldController::event_trigger(String event) {
    if (event.equals("laser")) {
      Serial.println(F("Controller: LASER event triggered"));
      field_comms->message("avr-building/1/laser-hit","1");
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
}

boolean BAVRFieldController::setup(const char* unique_id) {
  Serial.println(F("Controller setup..."));

  //Listen for return initialization call
  char buff2[128];
  memset(buff2, 0, 128);

  strcpy(buff2,"nodered/initialization/");
  strcat(buff2,unique_id);
  field_comms->subscribe(buff2);

  //Hand build JSON for now
  //Send request for initialization params (mostly node id)
  char buffer[128];
  memset(buffer, 0, 128);

  strcpy(buffer,"{\"uuid\":\"");
  strcat(buffer,unique_id);
  strcat(buffer,"\", \"timestamp\":10000000}");

  field_comms->message("edgenode/initialization",(const char*)buffer);
  return true;

}

