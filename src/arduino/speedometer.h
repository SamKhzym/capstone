#pragma once

#define _USE_MATH_DEFINES
#include <math.h>
#include <Arduino.h>

#define DEBUG 1 // if DEBUG flag defined, will send debug messages over serial console. need to turn off for production testing.

class Speedometer {

public:
    Speedometer();
    Speedometer(String name, int pinNum, float dutyCycleLowerThreshold_pct, float dutyCycleUpperThreshold_pct, float radius_m, float minSpeed_mps);
    ~Speedometer();
    float getSpeed(float ts_s);

private:
    String name = "DEFAULT";
    int pinNum = 0;
    float lastTimestep_s = 0;
    float lastLastTimestep_s = 0;
    bool prevMagFieldHigh = false;
    float dutyCycleLowerThreshold_pct = 0;
    float dutyCycleUpperThreshold_pct = 0;
    float lastDutyCycle_pct = 0;
    float lastSpeed_mps = 0;
    float wheelRadius_m = 0;
    float minSpeed_mps = 0;
    float timeout = 0;
    float pwmToDutyCycle();

};