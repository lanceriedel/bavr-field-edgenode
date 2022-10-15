#include "ether.hpp"

void ethernet_setup(byte mac_addr[]) {
   if (Ethernet.begin(mac_addr) == 0) {

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