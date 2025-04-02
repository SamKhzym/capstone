/** ACC algorithm
 * 
 * By: Nicholas P
 * Modified By: Pesara A.
 * Description: Implements and ACC control algorithm
 * */

#include "acc.h"

//Globals and parameters
AccParams accParams;
const float dt = 0.01f;
bool leadVehicleExists(float leadDist, float leadSpeed, float setSpeed) {
    return leadSpeed < setSpeed && leadDist < accParams.maxLeadDist;
}

void initPidParams(PidParams* params, float P, float I, float D) {
    params->P = P;
    params->I = I;
    params->D = D;
    params->errorSum = 0.0f;
    params->lastError = 0.0f;
}

float pidStep(PidParams* params, float err) {

    float pTerm = err * params->P;

    params->errorSum += err;
    float iTerm = params->errorSum * params->I;

    float derivative = (err - params->lastError) / dt;
    params->lastError = err;
    float dTerm = derivative * params->D;

    return pTerm + iTerm + dTerm;

}

void initAcc() {

    float speedKp = 2.5f;
    float speedKi = 1.0f;
    float speedKd = 0.04f;

    float distKp = 5.0f;
    float distKi = 2.0f;
    float distKd = 0.08f;

    initPidParams(&accParams.speedPid, speedKp, speedKi, speedKd);
    initPidParams(&accParams.distPid, distKp, distKi, distKd);
    accParams.maxLeadDist = 2.5f;
    accParams.timeGap = 5.0f;
    accParams.constDistGap = 1.0f;

}

uint8_t saturateActReq(float actReq) {

    uint8_t saturated = 0;

    if (actReq < 50){
        saturated = 0;
    }
    else if (actReq < 100){
        saturated = 100;
    }
    else if (actReq > 255) {
        saturated = 255;
    }
    else {
        saturated = (uint8_t)actReq;
    }

    return saturated;

}

uint8_t stepAcc(float hostVel, float leadVel, float setSpeed, float leadDist){

    // Decide between cruise and follow modes
    enum Mode mode = CRUISE; // defaults in cruise state
    bool lead_exists = leadVehicleExists(leadDist, leadVel, setSpeed);

    if (lead_exists) { //Future additions: safety critical mode, failure mode
        mode = FOLLOW;
        setSpeed = leadVel;
    }
    
    //Calculate PID terms
    float speed_error = setSpeed - hostVel;
    float dist_error = leadDist - hostVel * accParams.timeGap + accParams.constDistGap;
    float speed_req = pidStep(&accParams.speedPid, speed_error);
    float actReq;

    if (lead_exists) {
        float dist_req = pidStep(&accParams.distPid, dist_error);
        actReq = (speed_req + dist_req) / 2;
    }
    else {
        actReq = speed_req;
    }

    // pseudo-saturation
    uint8_t saturatedActReq = saturateActReq(actReq);

    return saturatedActReq;

}