/*
 Basic MQTT example

 This sketch demonstrates the basic capabilities of the library.
 It connects to an MQTT server then:
  - publishes "hello world" to the topic "outTopic"
  - subscribes to the topic "inTopic", printing out any messages
    it receives. NB - it assumes the received payloads are strings not binary

 It will reconnect to the server if the connection is lost using a blocking
 reconnect function. See the 'mqtt_reconnect_nonblocking' example for how to
 achieve the same result without blocking the main loop.
 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>
#include <ArduinoUniqueID.h>

#include "LaserDetect.hpp"
#include "LEDAnimations.hpp"


//for messaging
EthernetClient ethClient;
String unique_id ;
PubSubClient client(ethClient);

BAVRFieldComms fieldComms;

//for laser detecting
LaserDetect laser_detect(fieldComms);

// for leds
LEDAnimations led_animations;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,1,5);
IPAddress server(192, 168, 1, 112);

void callback(char* topic, byte* payload, unsigned int length) {
  fieldComms.callback(topic, payload,length);
}

void setup()
{
  Serial.begin(9600);
  char outputBuffer[10];
  uint8_t i = 0;
  for (; i < UniqueIDsize; i++)
  {
      outputBuffer[i] = (char)UniqueID[i];
  }
  i;;
  outputBuffer[i] = 0;
  unique_id = String(outputBuffer);
  Serial.print("BOARD_ID:"); Serial.println(unique_id);
  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);

  //leds
  led_animations.setup();
  //comms setup
  fieldComms.setup(unique_id, client);
  //laser
  laser_detect.laser_init();

}

void loop()
{
  if (!fieldComms.connected()) {
    fieldComms.reconnect();
  }
  fieldComms.loop();
  laser_detect.laser_detect();
  led_animations.ledanimate();
}
