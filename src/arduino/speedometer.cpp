#include "speedometer.h"
#include <math.h>

Speedometer::Speedometer(float dutyCycleThreshold_pct, float radius_m, float minSpeed_mps) {

    this->dutyCycleThreshold_pct = dutyCycleThreshold_pct;
    this->prevDutyCycle_pct = 0.5;
    this->lastTimestep_s = 0;
    this->lastSpeed_mps = 0;
    this->wheelRadius_m = radius_m;
    this->minSpeed_mps = minSpeed_mps;
    this->timeout = 1.2 * (2 * M_PI * this->wheelRadius_m) / this->minSpeed_mps;

}

float Speedometer::getSpeed(float currDutyCycle_pct, float ts_s) {

    // if duty cycle crossed max threshold this timestep, determine speed
    if (currDutyCycle_pct >= this->dutyCycleThreshold_pct && this->prevDutyCycle_pct < this->dutyCycleThreshold_pct) {
        float dt = ts_s - this->lastTimestep_s;
        float distanceTravelled = 2 * M_PI * this->wheelRadius_m;
        this->lastSpeed_mps = distanceTravelled / dt;
        this->lastTimestep_s = ts_s;
    }
    
    // determine if we have not received a tick for the maximum allowable timeout (speed = 0)
    if ((ts_s - this->lastTimestep_s) >= this->timeout) {
        this->lastSpeed_mps = 0;
    }
        
    // return speed
    return this->lastSpeed_mps;

} 