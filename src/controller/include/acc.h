/** ACC algorithm header file
 * 
 * Modified By: Pesara A.
 * Description: Implements structs, enums, consts, and function headers to 
 * be used by ACC.c
 * */

#include <stdint.h>
#include <stdbool.h>

#if defined(_MSC_VER)
    //  Microsoft 
    #define EXPORT __declspec(dllexport)
    #define IMPORT __declspec(dllimport)
#elif defined(__GNUC__)
    //  GCC
    #define EXPORT __attribute__((visibility("default")))
    #define IMPORT
#else
    //  do nothing and hope for the best?
    #define EXPORT
    #define IMPORT
    #pragma warning Unknown dynamic link import/export semantics.
#endif

//structs
typedef struct PidParams {
    float P;
    float I;
    float D;
    float errorSum;
    float lastError;
    float integralSaturation;
} PidParams;

typedef struct {
    float maxLeadDist;
    float timeGap;
    float constDistGap;
    PidParams speedPid;
    PidParams distPid;
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
void initPidParams(PidParams* params, float P, float I, float D, float integralSat);
float pidStep(PidParams* params, float err);
uint8_t saturateActReq(float actReq);

#ifdef EXPORT_DLL
EXPORT // mark function for dll export
#endif
void initAcc();

#ifdef EXPORT_DLL
EXPORT // mark function for dll export
#endif
uint8_t stepAcc(float hostVel, float leadVel, float setSpeed, float leadDist);
