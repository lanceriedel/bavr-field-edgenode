#ifndef LaserDetect_h
#define LaserDetect_h

#include "Adafruit_TCS34725.h"

class LaserDetect
{
public:
    LaserDetect();
    void laser_init();
    void laser_trigger();
    int8_t laser_detect();
    void calibrate() ;
    void reset();
    void set_diff(uint16_t diff);
    uint16_t get_lastest_readings();
    uint32_t get_lastest_avgk();
    uint32_t get_latest_diff();
    uint32_t get_latest_triggered_diff();

   // int8_t triggered();

private:
    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_50MS, TCS34725_GAIN_16X);

   // int led = 13;
    int avg_values_countdown = 5;
    uint32_t total_k = 0;
    uint32_t total_r = 0;

    uint32_t avg_k = 0;

    uint32_t avg_r=0;

    int32_t MAX_TEMP_DIFF=400;
    uint32_t last_diff;
    uint32_t last_triggered_diff;

    uint8_t hastriggered = -1;
    int MAX_WAIT =500;
    uint32_t lasttrigger = 0;
    uint16_t last_temp=0;


};

#endif
