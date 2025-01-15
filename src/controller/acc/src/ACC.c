/** ACC algorithm
 * 
 * By: Nicholas P
 * Modified By: Pesara A.
 * Description: Implements and ACC control algorithm
 * */
#include<stdio.h>

#include "ACC.h"

//Globals
float hostVel;
float hostVelPrev; //Host velocity in the previous time step
float integral = 0; //Needs to be global as it is used in the calculation of next step
float errorPrev = -999.0; //Previous value of PID error
float leadDistPrev = 0;
float overshoot = 0;
float leadTime = 0;

void vehDynamics(float torque){
    //torque will be a value from -1 to 1, where negative values represent braking torque and positive values represent acceleration

    //Calculate drag
    //CdA = 0.27
    float CdA = 0.55; //Default value for Drag Area
    float rho = 1.293; //Default for air density
    float mass = 1670.0; //Default of mass of vehicle

    float drag = -0.5*rho*CdA*hostVel*hostVel/mass; //Deceleration due to drag

    float hostAccel;
    if (torque >= 0){ //Braking is more powerful than acceleration. Will this break the PID? Probably
        hostAccel = drag + torque*4; //Assuming max acceleration is 4m/s; may want to make this a lookup table at some point
    }
    else if(torque < 0){
        hostAccel = drag + torque*7; //Assuming max deceleration is 7m/s; may want to make this a lookup table at some point
    }
    
    hostVel += hostAccel*timeStep; //Iterate the host velocity based on acceleration

    //Prevent negative velocity
    if (hostVel < 0){
        hostVel = 0;
    }
}

void initACC(float* errorPrev, float* derivative){
    if (*errorPrev == -999.0){ //AKA first time step
        *derivative = 0;
    }
}

float accAlgo(float hostVel, float leadVel, float setSpeed, float leadDist){
    //Decide between cruise and follow modes
    enum Mode mode; //0 = cruise, 1 = follow
    if (leadVel > setSpeed || leadDist > 500){ //Future additions: safety critical mode, failure mode
        mode = CRUISE;
        //Do nothing, this algorithm defaults in cruise mode
    }
    else{
        mode = FOLLOW;
        setSpeed = leadVel;
    }
    
    //Calculate PID terms
    float error = setSpeed - hostVel; //P
    integral += error*timeStep; //I
    float derivative = (error - errorPrev)/timeStep; //D

    //Set PID parameters

    pidParams pid;
    pid.P = 3;
    pid.I = 0.05;
    pid.D = 0.04;

    //Remove derivative from first time step
    initACC(&errorPrev, &derivative);

    //Set previous values for next time step
    errorPrev = error;

    //Implement PID controller
    uint8_t actReq = pid.P * error + pid.I * integral + pid.D * derivative;

    //Saturate actuation request value
    
    if (actReq < 50){
        actReq = 0;
    }
    else if (actReq < 100){
        actReq = 100;
    }

    // //Calculate overshoot, lead time, and steady state error
    // if (((*hostVelInit < setSpeed) && (error < overshoot)) || ((*hostVelInit > setSpeed) && (error > overshoot))){
    //     overshoot = error;
    // }
    // if (((*hostVelInit < setSpeed) && error > 0) || ((*hostVelInit > setSpeed) && error < 0)){
    //     leadTime += timeStep;
    // }


    // vehDynamics(actReq); //Recalculate vehicle speed

    //printf("%07.3f | %07.3f | %07.3f | %07.3f | %07.3f| %d\n", error, integral, derivative, torque, hostVel, mode);
    // printf("%07.3f | %07.3f | %07.3f | %07.3f | %07.3f| %d\n", error, torque, hostVel*3.6, leadDist, leadVel*3.6, mode);

    return actReq;
}



void leadDynamics(float* dynamics, float timeStamp, float leadDistInit, float leadVelInit, enum Function mode){
    //0 = Constant, 1 = Linear, 2 = Stepwise, 3 = Parabolic, 4 = Sinusoidal
    
    if (timeStamp == 0.0){
        leadDistPrev = leadDistInit;
    }

    float leadVel;
    float leadDist;

    if (mode == CONSTANT){ //Constant
        leadVel = leadVelInit;
    }

    if (mode == LINEAR){ //Linear
        float slope = -1; //CAL
        leadVel = leadVelInit + slope*timeStamp;
    }

    //Update lead distance based on relative velocity
    leadDist = leadDistPrev + (leadVel - hostVel) * timeStamp;
    leadDistPrev = leadDist;

    dynamics[0] = leadVel;
    dynamics[1] = leadDist;
}

int main (){
    //Define Calibrations
    float setSpeed = 50/3.6;
    float hostVelInit = 50/3.6;
    float leadVelInit = 50/3.6;
    float leadDistInit = 1000;
    int mode = LINEAR; //0 = Constant, 1 = Linear, 2 = Stepwise, 3 = Parabolic, 4 = Sinusoidal
    hostVel = hostVelInit;

    printf("Time| Error   | Torque  | HostVel | LeadDist| LeadVel|Mode\n");
    int steps = simTime/timeStep;
    for (int i = 0; i <= steps; i++){
        printf("%04.1f| ", i*timeStep);

        //Calculate the lead vehicle speed and distance
        float leadDyn[2];
        leadDynamics(leadDyn, i*timeStep, leadDistInit, leadVelInit, mode);

        //Initial Speed, Lead Velocity, Set Speed, Lead Distance
        accAlgo(&hostVelInit, leadDyn[0], setSpeed, leadDyn[1]);
        
    }

    printf("\nMax overshoot: %f\nLead Time: %f",-1*overshoot, leadTime);
    return 0;
}

//Time| Error   | Torque  | HostVel | LeadDist| LeadVel|Mode
