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
#include "Adafruit_TCS34725.h"



//for laser detecting
Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
int led = 13;
int avg_values_countdown = 20;
uint32_t total_k = 0;
uint32_t total_r = 0;
uint32_t avg_k = 0;
uint32_t avg_r=0;
const int32_t MAX_TEMP_DIFF=600;

// Update these with values suitable for your network.
byte mac[]    = {  0xDE, 0xED, 0xBA, 0xFE, 0xFE, 0xED };
IPAddress ip(192,168,1,5);
IPAddress server(192, 168, 1, 112);

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i=0;i<length;i++) {
    Serial.print((char)payload[i]);
  }
  Serial.println();
}

EthernetClient ethClient;
PubSubClient client(ethClient);

void reconnect() {
  // Loop until we're reconnected
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("arduinoClient")) {
      Serial.println("connected");
      // Once connected, publish an announcement...
      client.publish("outTopic","hello world");
      // ... and resubscribe
      client.subscribe("inTopic");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void laser_init() {
  Serial.println(" ");
  while (avg_values_countdown) {
    uint16_t r, g, b, c, colorTemp;
    int32_t lux;
    tcs.getRawData(&r, &g, &b, &c);
    // colorTemp = tcs.calculateColorTemperature(r, g, b);
    colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
    lux = tcs.calculateLux(r, g, b);

    Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
    Serial.print("C: "); Serial.print(c, DEC); Serial.print(" - ");
    Serial.print("LUx: "); Serial.print(lux); Serial.print(" - ");
    Serial.print("R: "); Serial.print(r, DEC); Serial.print(" -");
    Serial.print("G: "); Serial.print(g, DEC); Serial.print(" -");
    Serial.print("B: "); Serial.print(b, DEC); Serial.println(" -");

  
    total_k+=colorTemp;
    total_r+=r;
    delay(500);
    Serial.println("Calibrating... "); Serial.println(" ");
    avg_values_countdown--;
  }

  //Calculate the Average
  if (avg_k==0) {
    avg_k = total_k/20;
    avg_r = total_r/20;
    Serial.print("AVG K: "); Serial.print(avg_k, DEC); Serial.println(" ");
    Serial.print("AVG R: "); Serial.print(avg_r, DEC); Serial.println(" ");

    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(4000);               // wait for a second
    digitalWrite(led, LOW); 
  }


}

void laser_trigger() {
    client.publish("avr-building","shot-fired".);

    Serial.print("FLASH LED"); Serial.println(" ");

    digitalWrite(led, HIGH);   // turn the LED on (HIGH is the voltage level)
    delay(1000);               // wait for a second
    digitalWrite(led, LOW); 

}
void laser_detect() {
  uint16_t r, g, b, c, colorTemp;
  int32_t lux;
  

  tcs.getRawData(&r, &g, &b, &c);
  // colorTemp = tcs.calculateColorTemperature(r, g, b);
  colorTemp = tcs.calculateColorTemperature_dn40(r, g, b, c);
  lux = tcs.calculateLux(r, g, b);

  Serial.print("Color Temp: "); Serial.print(colorTemp, DEC); Serial.print(" K - ");
  Serial.print("C: "); Serial.print(c, DEC); Serial.print(" - ");
  Serial.print("LUx: "); Serial.print(lux); Serial.print(" - ");
  Serial.print("R: "); Serial.print(r, DEC); Serial.print(" -");
  Serial.print("G: "); Serial.print(g, DEC); Serial.print(" -");
  Serial.print("B: "); Serial.print(b, DEC); Serial.print(" -");


  int32_t dff_temp = avg_k-colorTemp;
  if (dff_temp<0) dff_temp = dff_temp * -1;
  if (dff_temp>100000) dff_temp = 0;
  //if ((abs(avg_r-r))>100) {
    Serial.print("DIFF K: "); Serial.print(dff_temp); Serial.println(" ");

  if (dff_temp>MAX_TEMP_DIFF) {
    laser_trigger();
  }

}

void setup()
{
  Serial.begin(9600);

  client.setServer(server, 1883);
  client.setCallback(callback);

  Ethernet.begin(mac, ip);
  // Allow the hardware to sort itself out
  delay(1500);
  laser_init();


}

void loop()
{
  if (!client.connected()) {
    reconnect();
  }
  client.loop();
  laser_detect();

}