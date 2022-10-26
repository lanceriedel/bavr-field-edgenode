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

   // FastLED.addLeds<WS2812, LED_PIN_TROUGH, GRB>(trough_leds[0], NUM_TROUGH_LED);
}

long TroughDetect::get_last_weight_stable() {
    return last_weight_stable;
}
int TroughDetect::bag_num() {
   return num_bags;
}

void TroughDetect::reset() {
    last_weight = 0;
    new_bag = false;
    num_bags = 0;
    hastriggered = false;
   trough_init();
}

boolean TroughDetect::triggered() {
   if (hastriggered) {
    hastriggered = 0;
    return true;
   } else
    return false;

}

void TroughDetect::trough_trigger() {

    hastriggered = true;
    Serial.print("Triggered bag in trough:"); Serial.println(num_bags);


    //todo: MQTT publish event according to schema
    //client->publish("avr-building","path-blocked");

    //todo: flash leds without delays




}

int TroughDetect::trough_detect() {


    long bgw=abs(wtsns.read());
    long diff_reading = bgw-last_weight;
    if (diff_reading>threshold_stablized) {
        Serial.print("Unstable Trough reading:");
        Serial.println(bgw);
        Serial.print("last reading :");
        Serial.println(last_weight);
        last_weight=bgw;
        return 0;
    }



    long diff = bgw-last_weight_stable;

    if (diff>threshold) {
        Serial.print("Trough detect:");
        Serial.println(bgw);
        Serial.print("last weight :");
        Serial.println(last_weight_stable);

        Serial.print("DIFF weight :");
        Serial.println(diff);

        num_bags=((bgw+(threshold/2))/bag_weight)>max_bags?max_bags:((bgw+(threshold/2))/bag_weight);
        trough_trigger();
    }

    last_weight_stable=bgw;


    return num_bags;
}

