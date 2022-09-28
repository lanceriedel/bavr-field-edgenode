#ifndef BallDetect_h
#define BallDetect_h

#include "Arduino.h"


#define BALL_COUNTER_PIN 4
#define TRIGGER_STATE 0 //high/low swap


class BallDetect
{
public:
    BallDetect();
    void ball_init();
    void ball_trigger();
    bool ball_detect();
    bool triggered();

private:
    bool hastriggered = false;
};

#endif
