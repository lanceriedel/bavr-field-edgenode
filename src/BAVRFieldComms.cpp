#ifndef BAVRFieldComms_cpp
#define BAVRFieldComms_cpp

#include "BAVRFieldComms.hpp"
#include <SPI.h>
#include <Wire.h>

BAVRFieldComms::BAVRFieldComms( )
{


}

void BAVRFieldComms::reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print(F("Attempting MQTT connection..."));

    // Attempt to connect
    if (client->connect("node-avrfield2")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      String hello = String("hello world:" + unique_id);

      client->publish("outTopic", hello.c_str());
      // ... and resubscribe
      client->subscribe("inTopic");
      client->subscribe("windowon");
      client->subscribe("windowoff");
      delay(1000);

    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
    }
        Serial.print("Finished MQTT connection...");

  }
}


boolean BAVRFieldComms::setup(String unique_id, PubSubClient* client) {
    this->client = client;
    return true;
    
}

boolean BAVRFieldComms::connected() {
  return client->connected();
}


boolean BAVRFieldComms::message(String topic, String messagestr) {

  client->publish(topic.c_str(), messagestr.c_str());
  Serial.print("Publish topic:"); Serial.print(topic); Serial.print(" message:"); Serial.println(messagestr);
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




