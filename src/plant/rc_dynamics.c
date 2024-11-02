#include "rc_dynamics.h"

vehDynamics dynamics;

void dynamicsInit() {

    dynamics.vehicleSpeed_mps = 0;

}

double dynamicsStep(int pwmCommand) {

    // if command is less than 100, no propulsion - vehicle slows down to 0
    if (pwmCommand < 100) {
        dynamics.vehicleSpeed_mps -= MAX_ACCEL_RATE_MPS2 / SAMPLE_RATE;
    }

    // if command is between 100-255, vehicle speeds up according to ~log curve
    // assumption : acceleration is a constant to allow car to ramp up to correct speed
    else {

        // logarithmic curve determined empirically
        double speedDelta = A * log(pwmCommand) + B - dynamics.vehicleSpeed_mps;
        double maxSpeedDelta = MAX_ACCEL_RATE_MPS2 / SAMPLE_RATE;
        int sign = speedDelta < 0 ? -1 : 1;

        double actualDelta = fabs(speedDelta) <= maxSpeedDelta ? speedDelta : maxSpeedDelta * sign;

        dynamics.vehicleSpeed_mps += actualDelta;
    }

    dynamics.vehicleSpeed_mps = fmax(dynamics.vehicleSpeed_mps, 0.0);
    
    return dynamics.vehicleSpeed_mps;

}