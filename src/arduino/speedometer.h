#define _USE_MATH_DEFINES
#include <math.h>
#include <Arduino.h>

class Speedometer {

public:
    Speedometer();
    Speedometer(int pinNum, float dutyCycleThreshold_pct, float radius_m, float minSpeed_mps);
    ~Speedometer();
    float getSpeed(float ts_s);

private:
    int pinNum;
    float lastTimestep_s;
    float prevDutyCycle_pct;
    float dutyCycleThreshold_pct;
    float lastSpeed_mps;
    float wheelRadius_m;
    float minSpeed_mps;
    float timeout;
    float pwmToDutyCycle();

};