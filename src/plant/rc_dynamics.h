#pragma once

#include "math.h"

//
#define SAMPLE_RATE 100.0
#define MIN_SPEED_MPS 0.20
#define MAX_SPEED_MPS 0.69
#define MAX_ACCEL_RATE_MPS2 1.38

// params determined by this curve: https://www.desmos.com/calculator/qzaprkqlwp
#define A 0.428483
#define B -1.68608

struct vehDynamics {
  double vehicleSpeed_mps;
};

typedef struct vehDynamics vehDynamics;

extern vehDynamics dynamics;

extern void dynamicsInit();
extern double dynamicsStep(int pwmCommand);