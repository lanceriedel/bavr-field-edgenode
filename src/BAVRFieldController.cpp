#include "BAVRFieldController.hpp"
#include <ArduinoJson.h>


BAVRFieldController::BAVRFieldController(LEDAnimations* led_animations, LaserDetect* laser_detect, BAVRFieldComms* field_comms, TroughDetect* trough_detect, BallDetect* ball_detect)
{
    this->laser_detect = laser_detect;
    this->led_animations = led_animations;
    this->field_comms = field_comms;
    this->trough_detect = trough_detect;
    this->ball_detect = ball_detect;
}

void BAVRFieldController::laser_hit_message(int hits, int whichone) {

  // Produce a minified JSON document
  const int capacity = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<capacity> doc;
  char output[128];
  doc["num_hits"] = hits;
  doc["timestamp"] = 100000;
  doc["node_id"] = (const char*)node_id;
  doc["side_id"] = whichone + 1;

  serializeJson(doc, output);


  char buff2[256];
  memset(buff2, 0, 256);

  strcpy(buff2,"edgenode/laserhit/");
  strcat(buff2,node_id);

  Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(buff2,(const char*)output);
}

void BAVRFieldController::ball_detect_message(int drops) {

  // Produce a minified JSON document
  const int capacity = JSON_OBJECT_SIZE(4);
  StaticJsonDocument<capacity> doc;
  char output[128];
  doc["num_drops"] = drops;
  doc["timestamp"] = 100000;
  doc["node_id"] = (const char*)node_id;
  doc["side_id"] = 0;

  serializeJson(doc, output);


  char buff2[256];
  memset(buff2, 0, 256);

  strcpy(buff2,"edgenode/balldrop/");
  strcat(buff2,node_id);

  Serial.print("message:"); Serial.print(buff2);Serial.print(" payload:"); Serial.println(output);

  field_comms->message(buff2,(const char*)output);
}


bool prefix(const char *pre, const char *str)
{
    return strncmp(pre, str, strlen(pre)) == 0;
}

void BAVRFieldController::subscribe_all() {
  char buff2[256];
  memset(buff2, 0, 256);

  strcpy(buff2,"nodered/firescore/");
  strcat(buff2, node_id);
  field_comms->subscribe(buff2);
  Serial.print(F("Subscribed to:")); Serial.println(buff2);

  //Subscribe to all messages for this
}

void BAVRFieldController::interrupt(int pin) {
  Serial.println("");Serial.print("interupt pin:"); Serial.println(pin);
  if (pin==ball_detect->get_pin()) {
    ball_detect->ball_trigger();
  }
}

//Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void BAVRFieldController::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print(F("Message arrived ["));
  Serial.print((topic));
  Serial.print(F("] "));
  Serial.println(length);

  char buffer[512];
  if (length>0) {
    strncpy(buffer, (const char*) payload, length);
    buffer[length] = 0;
  }

  if (prefix("nodered/firescore/", topic)) {
      StaticJsonDocument<32> doc;
    // Deserialize the JSON document
    DeserializationError error = deserializeJson(doc, buffer);

    // Test if parsing succeeds.
    if (error) {
      Serial.print(F("deserializeJson() failed: "));
      Serial.println(error.f_str());
      return;
    }

 
    int newfirescore = doc["firescore"];
    Serial.println(newfirescore);
    current_fire_score = newfirescore;
  }

  if (prefix("nodered/initialization",topic)) {
        memset(node_id, 0, 128);
        strcpy(node_id,buffer);
        Serial.print(F("Node id initialized: ")); Serial.println(node_id);
        //now that we know who we are, subscribe to our nodeid
        subscribe_all();
  }
  //make this better
    
  Serial.println(buffer);
  Serial.println();
}

void BAVRFieldController::event_trigger(const char* event, int whichone) {

  //TODO: 
  //Create a backoff and drop events if something is flooding the system
  if(strcmp(event, "laser") ==  0) {
      this->laser_hit_message(1, whichone);
    }
  if(strcmp(event, "trough") ==  0) {
      //Serial.println(F("Controller: Trough event triggered"));
      //field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
    }
  if(strcmp(event, "ball") ==  0) {
    this->ball_detect_message(1);
      //Serial.println(F("Controller: Trough event triggered"));
      //field_comms->message("avr-building/1/trough", String(trough_detect->bag_num())); //todo: update to match schema
    }

}

///Everybody do a loop   -- make sure no one is hogging the one thread please!!
void BAVRFieldController::loop() {
  
  field_comms->loop();
  int8_t trigger = laser_detect->laser_detect();
  if (trigger>=0) {
    event_trigger("laser",trigger);
  }
  led_animations->ledanimate();

  if(trough_detect->trough_detect())
    event_trigger("trough",0);

  if(ball_detect->ball_detect())
    event_trigger("ball",0);

}

boolean BAVRFieldController::setup(const char* unique_id) {
  Serial.println(F("Controller setup..."));

  //Listen for return initialization call
  char buff2[128];
  memset(buff2, 0, 128);

  strcpy(buff2,"nodered/initialization/");
  strcat(buff2,unique_id);
  field_comms->subscribe(buff2);


  //subscribe to nodered/reset/match
  memset(buff2, 0, 128);
  strcpy(buff2,"nodered/reset/match");
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

