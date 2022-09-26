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
#include "BAVRFieldComms.hpp"
#include "BAVRFieldController.hpp"


//for messaging
EthernetClient ethClient;
String unique_id ;
PubSubClient client(ethClient);

BAVRFieldComms field_comms;

//for laser detecting
LaserDetect laser_detect;
// for leds
LEDAnimations led_animations;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,1,5);
IPAddress server(192, 168, 1, 112);

//Where the real work gets handed out
BAVRFieldController controller(led_animations, laser_detect, field_comms);


//Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void callback(char* topic, byte* payload, unsigned int length) {
  controller.callback(topic, payload,length);
}

void ethernet_setup() {
   if (Ethernet.begin(mac) == 0) {

    Serial.println("Failed to configure Ethernet using DHCP");

    // Check for Ethernet hardware present

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println("Ethernet shield was not found.  Sorry, can't run without hardware. :(");
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }

    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println("Ethernet cable is not connected.");
    }

    // initialize the Ethernet device not using DHCP:
    //Ethernet.begin(mac, ip);
    Serial.println("Could not obtain IP Address.");
    exit(1);
  }

  // print your local IP address:
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());

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
  // Ethernet setup
  ethernet_setup();
  // Allow the hardware to sort itself out
  delay(1500);

  //pubsub init
  client.setServer(server, 1883);
  client.setCallback(callback);

  //leds
  led_animations.setup();
  //comms setup
  field_comms.setup(unique_id, client);
  //laser
  laser_detect.laser_init();

}

void loop()
{
  controller.loop();
}
