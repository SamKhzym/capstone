#include "speedometer.h"

#define NO_FIELD_DC 50.0

Speedometer::Speedometer() {}

//Added new parameter
Speedometer::Speedometer(String name, int pinNum, float dutyCycleLowerThreshold_pct, float dutyCycleUpperThreshold_pct, float radius_m, float num_mags, float minSpeed_mps) {

    this->name = name;
    this->pinNum = pinNum;
    this->dutyCycleLowerThreshold_pct = dutyCycleLowerThreshold_pct;
    this->dutyCycleUpperThreshold_pct = dutyCycleUpperThreshold_pct;
    this->wheelRadius_m = radius_m;
    this->minSpeed_mps = minSpeed_mps;
    this->num_mags = num_mags;
    this->timeout = 1.5 * (2 * M_PI * this->wheelRadius_m * this->wheelRadius_m) / (this->minSpeed_mps * this->num_mags);

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
    Serial.print("Duty_Cycle_" + this->name + ": ");
    Serial.println(currDutyCycle_pct);
#endif

    float currDutyCycleFilt_pct = (currDutyCycle_pct + this->lastDutyCycle_pct) / 2;

    // if we've passed the upper threshold, mark an event
    bool magFieldHigh = (currDutyCycleFilt_pct >= this->dutyCycleUpperThreshold_pct);

    // if duty cycle crossed max threshold this timestep, determine speed
    if (magFieldHigh && !this->prevMagFieldHigh) {
        float dt = ts_s - this->lastTimestep_s;
        float distanceTravelled = M_PI * this->wheelRadius_m * (1.0f / this->num_mags);
        this->lastSpeed_mps = distanceTravelled / dt;
        this->lastLastTimestep_s = this->lastTimestep_s;
        this->lastTimestep_s = ts_s;
    }
    
    // determine if we have not received a tick for the maximum allowable timeout (speed = 0)
    // also if we are not initialized
    if ((ts_s - this->lastTimestep_s) >= this->timeout || ts_s <= this->initTime_s) {
        this->lastSpeed_mps = 0;
    }

    // set prev magnetic field high flag
    this->prevMagFieldHigh = magFieldHigh;
    this->lastDutyCycle_pct = currDutyCycle_pct;
        
    // return speed
    return this->lastSpeed_mps;

} 