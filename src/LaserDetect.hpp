#include "Adafruit_TCS34725.h"
#include <PubSubClient.h>



class LaserDetect
{
public:
    LaserDetect(PubSubClient& client);
    void laser_init();
    void laser_trigger();
    void laser_detect();

private:
    Adafruit_TCS34725 tcs = Adafruit_TCS34725(TCS34725_INTEGRATIONTIME_24MS, TCS34725_GAIN_16X);
    int led = 13;
    int avg_values_countdown = 20;
    uint32_t total_k = 0;
    uint32_t total_r = 0;
    uint32_t avg_k = 0;
    uint32_t avg_r=0;
    uint32_t last_unfired_k = 0;
    const int32_t MAX_TEMP_DIFF=400;
    PubSubClient* client;

};