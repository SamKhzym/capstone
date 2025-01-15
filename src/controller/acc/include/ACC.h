/** ACC algorithm header file
 * 
 * Modified By: Pesara A.
 * Description: Implements structs, enums, consts, and function headers to 
 * be used by ACC.c
 * */

//structs
typedef struct pidParams {
    float P;
    float I;
    float D;
} pidParams;

struct accParams {
    pidParams speedPid;
};

//enums
enum Mode {
    CRUISE,
    FOLLOW
};

enum Function {
    CONSTANT,
    LINEAR,
    STEPWISE,
    PARABOLIC,
    SINUSOIDAL
};

//Calibratables
const float timeStep = 0.1; //Time step of system
const float simTime = 20.0; //Simulation time in seconds

//funtion headers
void vehDynamics(float torque);
void initACC(float* errorPrev, float* derivative);
float accAlgo(float* hostVelInit, float leadVel, float setSpeed, float leadDist);
void leadDynamics(float* dynamics, float timeStamp, float leadDistInit, float leadVelInit, enum Function mode);

bool determineLeadVehicleExists(float leadDistance, float leadSpeed, float setSpeed);
void initPidParams(struct PidParams* params, float P, float I, float D);
float pidStep(struct PidParams* params, float err);
void initACC();
uint8_t accController(float leadSpeed, float leadDistance, float egoSpeed, float setSpeed);
uint8_t saturateOutputCommand(uint8_t actReq, uint8_t actReqPrev);