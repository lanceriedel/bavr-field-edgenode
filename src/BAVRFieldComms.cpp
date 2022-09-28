#ifndef BAVRFieldComms_cpp
#define BAVRFieldComms_cpp

#include "BAVRFieldComms.hpp"
#include <SPI.h>
#include <Wire.h>

BAVRFieldComms::BAVRFieldComms( )
{


}

void BAVRFieldComms::subscribe(const char* topic) {
      client->subscribe(topic);
}

void BAVRFieldComms::reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print(F("Attempting MQTT connection..."));

    // Attempt to connect
    if (client->connect("node-avrfield2")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      //String hello = String("hello world:" + unique_id);

     // client->publish("outTopic", hello.c_str());
      // ... and resubscribe
      client->subscribe("nodered/initialization/#");
      client->subscribe("windowon");
      client->subscribe("windowoff");
      delay(1000);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);

    }
        Serial.print("Finished MQTT connection...");

  }
}


boolean BAVRFieldComms::setup(const char* unique_id, PubSubClient* client) {
    this->client = client;
     if (!client->connected()) {
      reconnect();
      delay(1150);
    } 
    this->unique_id = unique_id;
    return true;
    
}

boolean BAVRFieldComms::connected() {
  return client->connected();
}


boolean BAVRFieldComms::message(const char* topic, const char* messagestr) {
  //Serial.println(F("Publish topic:")); Serial.println(topic); Serial.println(F(" message:")); Serial.println(messagestr);

  client->publish(topic, messagestr);
  return true;
}

boolean BAVRFieldComms::loop() {
    if (!client->connected()) {
      reconnect();
    } 
   // Serial.println("-");
    //pubsub loop
    client->loop();
}

#endif




