
#include <SPI.h>
#include <Wire.h>


//#include <SPI.h>
// Ethernet - Version: Latest 
#include <Ethernet.h>

// Enter a MAC address and IP address for your controller below.
// The IP address will be dependent on your local network.
// gateway and subnet are optional:
byte mac[] = {
  0x00, 0xAA, 0xBB, 0xCC, 0xDE, 0x02
};
IPAddress ip(192, 168, 1, 177);
IPAddress myDns(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
IPAddress mqttserver(192, 168, 1, 5);



char SerialBuf[80];
String strFromSerial; // saving 1 line string
String strFromMobileAPP;
String strLocalIP;

#include <MQTT.h>
EthernetClient net;
MQTTClient client;

// it's a public name space, so open the key & secret value.
#define MQTT_DeviceName "AVR_NODE_1"

#define MQTT_topic_Message  "/Message"



void connect() {
  Serial.print("connecting...");

  while (!client.connect(MQTT_DeviceName)) {
    Serial.print(".");
    delay(1000);
  }

  Serial.println("\nconnected!");

  client.subscribe(MQTT_topic_Message);
  // client.unsubscribe(MQTT_topic_Message);
}

void MQTT_control_run()
{
  client.loop();

  if (!client.connected()) {
    connect();
  }

  // publish a message roughly every second.
  /*
  if (millis() - lastMillis > 10000) {
    lastMillis = millis();
    client.publish("/hello", "world");
  }*/

}



void messageReceived(String &topic, String &payload) {
  Serial.println("incoming: " + topic + " - " + payload);
  if (topic==MQTT_topic_Message){
    strFromMobileAPP = payload;
  }
}

void printText() {
  if (strFromMobileAPP.length())
  {
        Serial.print(strFromMobileAPP);

  }
}


void User_application_run()
{
  printText();
}


char* ip2CharArray(IPAddress ip) {
  static char a[16];
  sprintf(a, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
  return a;
}


void setup()   {                
  // You can use Ethernet.init(pin) to configure the CS pin
  Ethernet.init(10);  // Most Arduino shields
  //Ethernet.init(5);   // MKR ETH shield
  //Ethernet.init(0);   // Teensy 2.0
  //Ethernet.init(20);  // Teensy++ 2.0
  //Ethernet.init(5);  // ESP8266 with Adafruit Featherwing Ethernet
  //Ethernet.init(33);  // ESP32 with Adafruit Featherwing Ethernet

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
      while (!Serial) {
          ; // wait for serial port to connect. Needed for native USB port only
      }

  // start the Ethernet connection:
  Serial.println("Trying to get an IP address using DHCP");
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
    Ethernet.begin(mac, ip, myDns, gateway, subnet);
  }

  Ethernet.MACAddress(mac); // fill the MAC buffer
  Serial.print("The MAC address is: ");
  for (byte octet = 0; octet < 6; octet++) {
    Serial.print(mac[octet], HEX);
    if (octet < 5) {
      Serial.print('-');
    }
  }
  Serial.println();

  // print your local IP address:
  ip = Ethernet.localIP(); // update the IP address buffer
  Serial.print("My IP address: ");
  Serial.println(Ethernet.localIP());


 
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  strLocalIP = "IP: " + String(ip2CharArray(Ethernet.localIP()));
  strFromMobileAPP = strLocalIP;

  // set the MQTT broker address to MQTTClient
  client.begin(mqttserver, 1883, net);
  // set the callback function for receiving
  client.onMessage(messageReceived);
  // connnect to MQTT broker
  connect();
}

void loop() {
  MQTT_control_run();
  User_application_run();
}