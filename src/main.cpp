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
#include "LaserDetect.hpp"
#include "LEDAnimations.hpp"
#include "BAVRFieldComms.hpp"
#include "BAVRFieldController.hpp"
#include "ether.hpp"
#include "uuid.hpp"
#include "BAVRFreeMemory.hpp"
#include "config.hpp"

// UUID
UUID uuid;

// networking
EthernetClient ethClient;
PubSubClient client(ethClient);
BAVRFieldComms field_comms;
IPAddress ip_addr(mqtt_server[0],mqtt_server[1],mqtt_server[2],mqtt_server[3]); // MQTT server

// for leds
LEDAnimations led_animations;

// ball detector
BallDetect ball_detect;

// laser detector
LaserDetect laser_detect;


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

  // set up the LED animations (do this first so we can use the gutters as indicators)
  Serial.println(F("LED Animations setup..."));
  led_animations.setup();
  led_animations.boot_sequence(1);

  // setup uuid
  uuid.init();
  Serial.println(F("Board ID Comes from https://github.com/lanceriedel/burn-uuid-eeprom"));
  Serial.print(F("BOARD_ID:"));
  Serial.println(((char *)uuid.simpl_uuid));

  // set up ethernet
  Serial.println(F("Ethernet setup..."));
  byte macheader[3] = {0x90, 0xA2, 0xDA};
  byte mac[] = {
      macheader[0],
      macheader[1],
      macheader[2],
      uuid.uuid[3],
      uuid.uuid[4],
      uuid.uuid[5]};
  ethernet_setup(mac);
  Serial.print(F("My IP address: "));
  Serial.println(Ethernet.localIP());
  delay(1500); // Allow the hardware to sort itself out

  led_animations.boot_sequence(2);

  // set up the ball detector
  Serial.println(F("Ball Detector setup..."));
  ball_detect.ball_init(BALL_DROP_PIN);
  led_animations.boot_sequence(3);

  // set up the laser detector
  Serial.println(F("Laser Detector setup..."));
  laser_detect.laser_init();
  led_animations.boot_sequence(4);


  led_animations.boot_sequence(5);

  Serial.println(F("Pubsub setup..."));
  // pubsub init
  client.setServer(ip_addr, mqtt_port);
  client.setCallback(callback);
  client.setBufferSize(1024);
  delay(1500);
  led_animations.boot_sequence(6);

  // comms setup
  Serial.println(F("Comms setup..."));
  byte *suuid = uuid.simpl_uuid;
  field_comms.setup((const char *)suuid, &client);
  led_animations.boot_sequence(7);

  Serial.println(F("Setup Done begin loops..."));
  pinMode(HEATER_PIN,OUTPUT);
  digitalWrite(HEATER_PIN, LOW);


  pinMode(LED_PIN_MOS1,OUTPUT);
  // digitalWrite(LASER_LIGHT_PIN, LOW);

  controller = new BAVRFieldController(&led_animations, &laser_detect, &field_comms, &ball_detect);
  Serial.println(F("Controller created..."));

  controller->set_heater_pin(HEATER_PIN);

  delay(1500);
  Serial.println(F("Controller setup..."));

  controller->setup((const char *)suuid);
  attachInterrupt(digitalPinToInterrupt(BALL_DROP_PIN), interruptPinBallDrop, FALLING);

  Serial.println(F("LED boot_sequence(0)..."));

  led_animations.boot_sequence(0);
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
    Serial.print(F("LOOPTIME: "));
    Serial.print(duration);
    Serial.print(F("\tWORST LOOP TIME: "));
    Serial.print(worst_loop_time);
    Serial.print(F("\tWORST MEMORY: "));
    Serial.println(controller->worst_memory);
  }
}
