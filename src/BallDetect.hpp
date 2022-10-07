#ifndef BallDetect_h
#define BallDetect_h

#include "Arduino.h"


#define BALL_COUNTER_PIN 0
#define TRIGGER_STATE 0 //high/low swap


class BallDetect
{
public:
    BallDetect();
    void ball_init(int pin);
    void ball_trigger();
    bool ball_detect();
    int get_pin();

private:
    bool hastriggered = false;
    int pin = 0;
    int sensorState = 0, lastState=0;
    uint32_t lasttrigger = 0;
    int MAX_WAIT =2;
};

#endif
