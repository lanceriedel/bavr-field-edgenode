#include "TroughDetect.hpp"
// #include <SPI.h>
// #include <Wire.h>

TroughDetect::TroughDetect()
{
}

void TroughDetect::trough_init() {
    Serial.println(" ");
    Serial.println("Weight Sensor Tare");
    delay(100); //lets values stabilize before zeroing
    wtsns.tare();

    FastLED.addLeds<WS2812, LED_PIN_TROUGH, GRB>(trough_leds[0], NUM_TROUGH_LED);
}

int TroughDetect::bag_num() {
   return num_bags;
}


void TroughDetect::trough_trigger() {
    
    

    //todo: MQTT publish event according to schema
    //client->publish("avr-building","path-blocked");

    //todo: flash leds without delays




}

int TroughDetect::trough_detect() {

    long bgw=abs(wtsns.read());
  
    if (bgw-last_weight>threshold) {
        num_bags=((bgw+(threshold/2))/bag_weight)>max_bags?max_bags:((bgw+(threshold/2))/bag_weight);
        trough_trigger();
    }

    last_weight=bgw;

    return num_bags;
}

