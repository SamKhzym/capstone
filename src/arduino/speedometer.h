#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <Arduino.h>

#define DEBUG 1 // if DEBUG flag defined, will send debug messages over serial console. need to turn off for production testing.

class Speedometer {

public:
    Speedometer();
    Speedometer(int pinNum, float dutyCycleThreshold_pct, float radius_m, float minSpeed_mps);
    ~Speedometer();
    float getSpeed(float ts_s);

private:
    int pinNum;
    float lastTimestep_s;
    float lastLastTimestep_s;
    bool prevMagFieldHigh;
    float dutyCycleThreshold_pct;
    float lastDutyCycle_pct;
    float lastSpeed_mps;
    float wheelRadius_m;
    float minSpeed_mps;
    float timeout;
    float pwmToDutyCycle();

};