#ifndef TroughDetect_h
#define TroughDetect_h

#include "GyverHX711.h"
#include "FastLED.h"

#define DATA_PIN_TROUGH 3
#define CLOCK_PIN_TROUGH 4


class TroughDetect
{
public:
    TroughDetect();
    void trough_init();
    void trough_trigger();
    boolean triggered();
    void trough_tare();
    int trough_detect();
    int bag_num();
    void reset();

private:
    GyverHX711 wtsns = GyverHX711(DATA_PIN_TROUGH,CLOCK_PIN_TROUGH,HX_GAIN64_A);
    int max_bags = 3;
    long last_weight = 0;
    boolean new_bag = false;
    int num_bags = 0;
    long bag_weight = 65000;
    long threshold = 25000; //sensor is sensistive... lol. bit of a threshold to help
    bool hastriggered = false;

};

#endif