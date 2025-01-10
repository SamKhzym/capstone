#include "speedometer.h"

Speedometer::Speedometer() {

    this->dutyCycleThreshold_pct = 0;
    this->prevDutyCycle_pct = 0.5;
    this->lastTimestep_s = 0;
    this->lastSpeed_mps = 0;
    this->wheelRadius_m = 0;
    this->minSpeed_mps = 0;
    this->timeout = 0;

}

Speedometer::Speedometer(int pinNum, float dutyCycleThreshold_pct, float radius_m, float minSpeed_mps) {

    this->pinNum = pinNum;
    this->dutyCycleThreshold_pct = dutyCycleThreshold_pct;
    this->prevDutyCycle_pct = 0.5;
    this->lastTimestep_s = 0;
    this->lastSpeed_mps = 0;
    this->wheelRadius_m = radius_m;
    this->minSpeed_mps = minSpeed_mps;
    this->timeout = 1.2 * (2 * M_PI * this->wheelRadius_m) / this->minSpeed_mps;

}

float Speedometer::pwmToDutyCycle() {

    unsigned long highTime = pulseIn(this->pinNum, HIGH); // Measure HIGH pulse width
    unsigned long lowTime = pulseIn(this->pinNum, LOW);  // Measure LOW pulse width

    float dutyCycle = 0;

    if (highTime > 0 && lowTime > 0) {
        dutyCycle = (highTime * 100.0) / (highTime + lowTime);
    }

    return dutyCycle;

}

float Speedometer::getSpeed(float ts_s) {

    float currDutyCycle_pct = pwmToDutyCycle();

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