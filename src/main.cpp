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
#include "ether.hpp"
#include "uuid.hpp"

// UUID
UUID uuid;

// networking
EthernetClient ethClient;
PubSubClient client(ethClient);
BAVRFieldComms field_comms;
IPAddress server(192, 168, 1, 112); // MQTT server

// for leds
LEDAnimations led_animations;

// ball detector
uint8_t BALL_DROP_PIN = 2;
BallDetect ball_detect;

// laser detector
LaserDetect laser_detect;

// trough
TroughDetect trough_detect;

// Where the real work gets handed out
BAVRFieldController *controller;
void interruptPinBallDrop()
{
  controller->interrupt(BALL_DROP_PIN);
}

// Handle all of the MQTT Messages -- they are being handed off to the controller to do the work
void callback(char *topic, byte *payload, unsigned int length)
{
  controller->callback(topic, payload, length);
}

void setup()
{
  // set up serial
  Serial.begin(115200);

  // setup uuid
  uuid.init();
  Serial.println("Board ID Comes from https://github.com/lanceriedel/burn-uuid-eeprom");
  Serial.print(F("BOARD_ID:"));
  Serial.println(((char *)uuid.simpl_uuid));

  // set up ethernet
  Serial.println(F("Ethernet setup..."));
  byte mac[] = {
      uuid.uuid[0],
      uuid.uuid[1],
      uuid.uuid[2],
      uuid.uuid[3],
      uuid.uuid[4],
      uuid.uuid[5]};
  ethernet_setup(mac);
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());
  delay(1500); // Allow the hardware to sort itself out

  // set up the LED animations
 // Serial.println(F("LED Animations setup..."));
  //led_animations.setup();

  // set up the ball detector
  //Serial.println(F("Ball Detector setup..."));
  //ball_detect.ball_init(BALL_DROP_PIN);

  // set up the laser detector
  Serial.println(F("Laser Detector setup..."));
  laser_detect.laser_init();

  // set up the scale (trough)
 // Serial.println(F("Trough setup..."));
 // trough_detect.trough_init();

  Serial.println(F("Pubsub setup..."));
  // pubsub init
  client.setServer(server, 1883);
  client.setCallback(callback);
  client.setBufferSize(1512);
  delay(1500);

  // comms setup
  Serial.println(F("Comms setup..."));
  byte *suuid = uuid.simpl_uuid;
  field_comms.setup((const char *)suuid, &client);

  Serial.println(F("Setup Done begin loops...")); 

  controller = new BAVRFieldController(&led_animations, &laser_detect, &field_comms, &trough_detect, &ball_detect);

  delay(1500);
  controller->setup((const char *)suuid);
  attachInterrupt(digitalPinToInterrupt(BALL_DROP_PIN), interruptPinBallDrop, FALLING);
}


unsigned long start_time = 0;
unsigned long end_time = 0;
unsigned long last_print_time = 0;
unsigned long worst_loop_time = 0;

void loop()
{
  // Serial.println(".");
  start_time = millis();
  controller->loop();
  end_time = millis();
  // Serial.println(digitalRead(BALL_DROP_PIN));
  unsigned long duration = end_time - start_time;
  if (duration > worst_loop_time)
  {
    worst_loop_time = duration;
  }
  if (millis() - last_print_time > 2000)
  {
    last_print_time = millis();
    Serial.print("LOOPTIME: ");
    Serial.print(duration);
    Serial.print("\tWORST LOOP TIME: ");
    Serial.println(worst_loop_time);
  }
}
