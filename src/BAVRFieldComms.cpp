#include "BAVRFieldComms.hpp"
#include <SPI.h>
#include <Wire.h>

BAVRFieldComms::BAVRFieldComms()
{
}

void BAVRFieldComms::callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  String topicstr(topic);
  if (topicstr.equals("windowon")) {
      led_animations.ledanimate(1);
      Serial.println("windowon msg");
  }

  if (topicstr.equals("windowoff")) {
      led_animations.ledanimate(0);
      
      Serial.println("windowoff msg");
  }
  
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

boolean BAVRFieldComms::setup(String unique_id, PubSubClient& client) {
    this->client = &client;
    

}

boolean BAVRFieldComms::connected() {
    return client->connected();
}

void BAVRFieldComms::reconnect() {
  // Loop until we're reconnected
  while (!client->connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client->connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      String hello = String("hello world:" + unique_id);

      client->publish("outTopic", hello.c_str());
      // ... and resubscribe
      client->subscribe("inTopic");
      client->subscribe("windowon");
      client->subscribe("windowoff");

    } else {
      Serial.print("failed, rc=");
      Serial.print(client->state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

boolean BAVRFieldComms::loop() {
    client->loop();
}



