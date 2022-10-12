#ifndef LaserDetect_h
#define LaserDetect_h

#include "Adafruit_TCS34725.h"

class LaserDetect
{
public:
    LaserDetect();
    void laser_init();
    void laser_trigger(uint8_t);
    int8_t laser_detect();
    void calibrate() ;
    void reset();
   // int8_t triggered();

private:
    Adafruit_TCS34725 tcs[4] = {Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X),
                           Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X)};





   // int led = 13;
    int avg_values_countdown = 5;
    uint32_t total_k[4]={0,0,0,0};
    //uint32_t total_k = 0;
    uint32_t total_r[4]={0,0,0,0};
   // uint32_t total_r = 0;

    //uint32_t avg_k = 0;
    uint32_t avg_k[4]={0,0,0,0};

    //uint32_t avg_r=0;
    uint32_t avg_r[4]={0,0,0,0};

    const int32_t MAX_TEMP_DIFF=600;
    uint8_t hastriggered = -1;
    int MAX_WAIT =500;
    uint32_t lasttrigger = 0;
    const int8_t NUM_SENSORS = 2;

};

#endif
