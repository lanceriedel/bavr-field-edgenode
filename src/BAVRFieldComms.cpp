#ifndef BAVRFieldComms_cpp
#define BAVRFieldComms_cpp

#include "BAVRFieldComms.hpp"
#include <SPI.h>
#include <Wire.h>

BAVRFieldComms::BAVRFieldComms( )
{


}

boolean BAVRFieldComms::setup(String unique_id, PubSubClient* client) {
    this->client = client;
    return true;
    
}

boolean BAVRFieldComms::connected() {
}


boolean BAVRFieldComms::message(String topic, String messagestr) {

  client->publish(topic.c_str(), messagestr.c_str());
  Serial.print("Publish topic:"); Serial.print(topic); Serial.print(" message:"); Serial.println(messagestr);
}

boolean BAVRFieldComms::loop() {
    Serial.println("comms loop");
}

#endif




