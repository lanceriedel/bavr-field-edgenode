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

    //give unique mqtt id
    char mqtt_id[24];
    strcpy(mqtt_id, "node-");
    strcat(mqtt_id, unique_id);

    // Attempt to connect
    if (client->connect((const char*) mqtt_id)) {
      Serial.println("connected");
      // client->subscribe("nodered/initialization/#");

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

boolean BAVRFieldComms::connected() {
  return client->connected();
}

boolean BAVRFieldComms::setup(const char* unique_id, PubSubClient* client) {
    this->client = client;
    this->unique_id = unique_id;
    if (!connected()) {
      reconnect();
      delay(1150);
    }
    return true;

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




