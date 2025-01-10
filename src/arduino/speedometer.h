#include <cstdint>

#define _USE_MATH_DEFINES
#include <math.h>

class Speedometer {

public:
    Speedometer(float dutyCycleThreshold_pct, float radius_m, float minSpeed_mps);
    ~Speedometer();
    float getSpeed(float currDutyCycle_pct, float ts_s);

private:
    float lastTimestep_s;
    float prevDutyCycle_pct;
    float dutyCycleThreshold_pct;
    float lastSpeed_mps;
    float wheelRadius_m;
    float minSpeed_mps;
    float timeout;

};