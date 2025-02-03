#include "ACC.h"

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
