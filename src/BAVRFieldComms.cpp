#ifndef BAVRFieldComms_cpp
#define BAVRFieldComms_cpp

#include "BAVRFieldComms.hpp"
#include <SPI.h>
#include <Wire.h>

BAVRFieldComms::BAVRFieldComms( )
{
}

void BAVRFieldComms::subscribe(const char* topic) {
  Serial.print(F("Subscribe to topic:"));Serial.println(topic);
  client->subscribe(topic);
}

void BAVRFieldComms::reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print(F("Attempting MQTT connection..."));

    // Attempt to connect

    if (client->connect(unique_id)) {
      Serial.println(F("connected"));
      needs_subscriptions = true;
    } else {
      Serial.print(F("failed, rc="));
      Serial.print(client->state());
      Serial.println(F(" try again in 5 seconds"));
      // Wait 5 seconds before retrying
      delay(5000);

    }
    Serial.print(F("Finished MQTT connection..."));

  }
}


boolean BAVRFieldComms::setup(const char* unique_id, PubSubClient* client) {
    this->client = client;
    this->unique_id = unique_id;

     if (!client->connected()) {
      reconnect();
      delay(1150);
    }
    return true;

}

boolean BAVRFieldComms::connected() {
  return client->connected();
}


boolean BAVRFieldComms::message(const char* topic, const char* messagestr) {
  Serial.println(F("Publish topic:")); Serial.println(topic); Serial.println(F(" message:")); Serial.println(messagestr);
  client->publish(topic, messagestr);
  return true;
}

void BAVRFieldComms::loop() {
    if (!client->connected()) {
      reconnect();
    }
    client->loop();
}

#endif




