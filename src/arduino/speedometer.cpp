#include "speedometer.h"

#define NO_FIELD_DC 50.0

Speedometer::Speedometer() {

    this->dutyCycleThreshold_pct = 0;
    this->prevMagFieldHigh = false;
    this->lastTimestep_s = 0;
    this->lastLastTimestep_s = 0;
    this->lastSpeed_mps = 0;
    this->wheelRadius_m = 0;
    this->minSpeed_mps = 0;
    this->timeout = 0;

}

Speedometer::Speedometer(int pinNum, float dutyCycleThreshold_pct, float radius_m, float minSpeed_mps) {

    this->pinNum = pinNum;
    this->dutyCycleThreshold_pct = dutyCycleThreshold_pct;
    this->prevMagFieldHigh = false;
    this->lastTimestep_s = 0;
    this->lastLastTimestep_s = 0;
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

#if DEBUG
    Serial.print("Duty_Cycle: ");
    Serial.println(currDutyCycle_pct);
#endif

    float currDutyCycleFilt_pct = (currDutyCycle_pct + this->lastDutyCycle_pct) / 2;

    // second clause only necessary if magnets are mounted in opposite directions, resulting in spikes in different directions
    bool magFieldHigh = (fabs(NO_FIELD_DC - currDutyCycleFilt_pct) >= this->dutyCycleThreshold_pct);

    // if duty cycle crossed max threshold this timestep, determine speed
    if (magFieldHigh && !this->prevMagFieldHigh) {
        float dt = ts_s - this->lastLastTimestep_s;
        float distanceTravelled = 2 * M_PI * this->wheelRadius_m * (2.0f / 5.0f);
        this->lastSpeed_mps = distanceTravelled / dt;
        this->lastLastTimestep_s = this->lastTimestep_s;
        this->lastTimestep_s = ts_s;
    }
    
    // determine if we have not received a tick for the maximum allowable timeout (speed = 0)
    if ((ts_s - this->lastTimestep_s) >= this->timeout) {
        this->lastSpeed_mps = 0;
    }

    // set prev magnetic field high flag
    this->prevMagFieldHigh = magFieldHigh;
    this->lastDutyCycle_pct = currDutyCycle_pct;
        
    // return speed
    return this->lastSpeed_mps;

} 