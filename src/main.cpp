/*
BAVR Field Edge node - arduino uno code for controlling and edgenode.  Need and Ethernet shield and expects
to connect to an MQTT host at IPAddress server(192, 168, 1, 112) (or change appropriately)

NOTE -  no delays should be anywhere in the loops -- will work to make sure that this is true especially in LED animations

 
*/

#include <SPI.h>
#include <Ethernet.h>
#include <PubSubClient.h>
#include <Wire.h>

#include "BallDetect.hpp"
#include "TroughDetect.hpp"
#include "LaserDetect.hpp"
#include "LEDAnimations.hpp"
#include "BAVRFieldComms.hpp"
#include "BAVRFieldController.hpp"
#include <EEPROM.h>

//for messaging
EthernetClient ethClient;
PubSubClient client(ethClient);

BAVRFieldComms field_comms;

//for laser detecting
LaserDetect laser_detect;
// for leds
LEDAnimations led_animations;

TroughDetect trough_detect;
BallDetect ball_detect;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
//MQTT Server Address
IPAddress server(192, 168, 1, 112);
char unique_id[32];
uint8_t BALL_DROP_PIN = 2;

//Where the real work gets handed out
 BAVRFieldController* controller;



//Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void callback(char* topic, byte* payload, unsigned int length) {
  controller->callback(topic, payload,length);
}


void interruptPinBallDrop() {
  controller->interrupt(BALL_DROP_PIN);
}

void ethernet_setup() {
   if (Ethernet.begin(mac) == 0) {

    Serial.println(F("Failed to configure Ethernet using DHCP"));

    // Check for Ethernet hardware present

    if (Ethernet.hardwareStatus() == EthernetNoHardware) {
      Serial.println(F("Ethernet shield was not found.  Sorry, can't run without hardware. :("));
      while (true) {
        delay(1); // do nothing, no point running without Ethernet hardware
      }
    }

    if (Ethernet.linkStatus() == LinkOFF) {
      Serial.println(F("Ethernet cable is not connected."));
    }

    // initialize the Ethernet device not using DHCP:
    //Ethernet.begin(mac, ip);
    Serial.println(F("Could not obtain IP Address."));
    exit(1);
  }

}

void setup()
{
  pinMode(BALL_DROP_PIN, INPUT_PULLUP);     
  digitalWrite(BALL_DROP_PIN, HIGH); // turn on the pullup
  Serial.begin(9600);
 // String idstr;
  char b[8];
  memset(b, 0, 8);
  memset(unique_id, 0, 32);

  
  snprintf(b, 8, "%lX",(long)EEPROM.read(0));
  strcpy(unique_id, b);
  memset(b, 0, 8);
  snprintf(b, 8, "%lX",(long)EEPROM.read(1));
  strcat(unique_id, b);
  memset(b, 0, 8);
  snprintf(b, 8, "%lX",(long)EEPROM.read(2));
  strcat(unique_id, b);
  memset(b, 0, 8);
  snprintf(b, 8, "%lX",(long)EEPROM.read(3));
  strcat(unique_id, b);

  Serial.println("Board ID Comes from https://github.com/lanceriedel/burn-uuid-eeprom");
  Serial.print(F("BOARD_ID:")); Serial.println((unique_id));
  // Ethernet setup
  delay(500);

   ethernet_setup();
  // print your local IP address:
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());
  // Allow the hardware to sort itself out
  delay(1500);

  Serial.println(F("Pubsub setup..."));
  //pubsub init
  client.setServer(server, 1883);
  client.setCallback(callback);
  client.setBufferSize(1512);
  delay(1500);


  //leds
  Serial.println(F("LED Animations setup..."));
  led_animations.setup();
  //comms setup
  Serial.println(F("Comms setup..."));
  field_comms.setup((const char*)unique_id, &client);
  //laser
  Serial.println(F("Laser Detector setup..."));

  laser_detect.laser_init();

  Serial.println(F("Trough setup..."));

  trough_detect.trough_init();

  Serial.println(F("Ball Detector setup..."));
  // initialize the sensor pin as an input:
 

  ball_detect.ball_init(BALL_DROP_PIN);


  Serial.println(F("Setup Done begin loops..."));

  controller = new BAVRFieldController(&led_animations, &laser_detect,  &field_comms, &trough_detect, &ball_detect);

  delay(1500);
  controller->setup(unique_id);
  attachInterrupt(digitalPinToInterrupt(BALL_DROP_PIN), interruptPinBallDrop, FALLING);


}



void loop()
{
  //Serial.println(".");
  controller->loop();
  //Serial.println(digitalRead(BALL_DROP_PIN));

}
