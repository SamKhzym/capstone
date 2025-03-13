/** ACC algorithm header file
 * 
 * Modified By: Pesara A.
 * Description: Implements structs, enums, consts, and function headers to 
 * be used by ACC.c
 * */

#include <stdint.h>
#include <stdbool.h>

//structs
typedef struct PidParams {
    float P;
    float I;
    float D;
    float errorSum;
    float lastError;
} PidParams;

typedef struct {
    float maxLeadDist;
    PidParams speedPid;
} AccParams;

//enums
enum Mode {
    CRUISE,
    FOLLOW
};

extern const float dt;
extern AccParams accParams;

// function headers
bool leadVehicleExists(float leadDist, float leadSpeed, float setSpeed);
void initPidParams(PidParams* params, float P, float I, float D);
float pidStep(PidParams* params, float err);
uint8_t saturateActReq(float actReq);

#ifdef EXPORT_DLL
__declspec(dllexport) // mark function for dll export
#endif
void initAcc();

#ifdef EXPORT_DLL
__declspec(dllexport) // mark function for dll export
#endif
uint8_t stepAcc(float hostVel, float leadVel, float setSpeed, float leadDist);
