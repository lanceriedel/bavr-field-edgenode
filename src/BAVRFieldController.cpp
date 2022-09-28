#include "BAVRFieldController.hpp"
#include <ArduinoJson.h>


BAVRFieldController::BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms, TroughDetect* trough_detect)
{
    this->laser_detect = laser_detect;
    this->led_animations = led_animations;
    this->field_comms = field_comms;
    this->trough_detect = trough_detect;
}

void BAVRFieldController::laser_hit_message(int hits) {
  //Hand build JSON for now
  //Send request for initialization params (mostly node id)
  //Serial.print("nodeid:"); Serial.println(node_id);
  // char buffer[256];
  // memset(buffer, 0, 256);

  // strcpy(buffer,"{\"hits\":\"");
  // char buffer2[10];
  // memset(buffer2, 0, 10);

  // snprintf(buffer2, 10, "%d", hits);

  // strcat(buffer,buffer2);
  // strcat(buffer,"\", \"timestamp\":10000000}");

  // Produce a minified JSON document
  const int capacity = JSON_OBJECT_SIZE(2);
  StaticJsonDocument<capacity> doc;
  char output[128];
  doc["hits"] = hits;
  doc["timestamp"] = 100000;

  serializeJson(doc, output);


  char buff2[256];
  memset(buff2, 0, 256);

  strcpy(buff2,"edgenode/laserhit/");
  strcat(buff2,node_id);

  Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(buff2,(const char*)output);
}


//Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void BAVRFieldController::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print((topic));
  Serial.print(F("] "));
  Serial.println(length);
  String topicstr(topic);
  char buffer[512];

  if (length>0) {
    strncpy(buffer, (const char*) payload, length);
    buffer[length] = 0;
  }

  if (topicstr.startsWith("led")) {
    
  }

  if (strcmp(topic,"nodered/initialization")==0) {
        memset(node_id, 0, 128);
        strcpy(node_id,buffer);
        Serial.print(F("Node id initialized: ")); Serial.println(node_id);
          Serial.print("nodeid:"); Serial.println(node_id);

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

void BAVRFieldController::event_trigger(const char* event) {

  //TODO: 
  //Create a backoff and drop events if something is flooding the system
  if(strcmp(event, "laser") ==  0) {
      this->laser_hit_message(1);
    }
  if(strcmp(event, "trough") ==  0) {
      //Serial.println(F("Controller: Trough event triggered"));
      //field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
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

