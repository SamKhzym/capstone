#include "comm_layer.h"
#include "stdio.h"

#ifdef ARDUINO
#include <Arduino.h>
#endif

void formatFloatString(char* buffer, float value, int length, int numDecimals) {

    snprintf(buffer, length, "%*.*f", length-numDecimals-1, numDecimals, value);
    
}

void packSpeedCommandPayload(const struct SpeedCommandPayload* payload, char* payloadBuffer) {
    sprintf(payloadBuffer, "<%3d>", payload->speedCommand_pwm);
}

void unpackSpeedCommandPayload(const char* payloadBuffer, struct SpeedCommandPayload* payload) {
    sscanf(payloadBuffer, "<%d>", &payload->speedCommand_pwm);
}

void packVehicleSpeedPayload(const struct VehicleSpeedPayload* payload, char* payloadBuffer) {
    char speedString[8];
    formatFloatString(speedString, payload->vehicleSpeed_mps, 8, 3);
    sprintf(payloadBuffer, "<%s>", speedString);
}

void unpackVehicleSpeedPayload(const char* payloadBuffer, struct VehicleSpeedPayload* payload) {
    sscanf(payloadBuffer, "<%f>", &payload->vehicleSpeed_mps);
}

void packVehicleSpeedExtendedPayload(const struct VehicleSpeedExtendedPayload* payload, char* payloadBuffer) {
    char speedString[6];
    formatFloatString(speedString, payload->egoSpeed_mps, 6, 3);
    
    sprintf(payloadBuffer, "<%s|%1d|%2d|%3d>", speedString, payload->speedFaultActive, payload->rc, payload->crc);
}

void unpackVehicleSpeedExtendedPayload(const char* payloadBuffer, struct VehicleSpeedExtendedPayload* payload) {
    sscanf(payloadBuffer, "<%f|%d|%d|%d>", 
        &payload->egoSpeed_mps,
        &payload->speedFaultActive,
        &payload->rc,
        &payload->crc
    );
}

void packEnvironmentInfoPayload(const EnvironmentInfoPayload* payload, char* payloadBuffer) {
    char leadSpeedString[6], leadDistanceString[7], setSpeedString[6];
    formatFloatString(leadSpeedString, payload->leadSpeed_mps, 6, 3);
    formatFloatString(leadDistanceString, payload->leadDistance_m, 7, 3);
    formatFloatString(setSpeedString, payload->setSpeed_mps, 6, 3);

    sprintf(payloadBuffer, "<%s|%s|%s>", 
        leadSpeedString, 
        leadDistanceString, 
        setSpeedString
    );
}

void unpackEnvironmentInfoPayload(const char* payloadBuffer, EnvironmentInfoPayload* payload) {
    sscanf(payloadBuffer, "<%f|%f|%f>", 
        &payload->leadSpeed_mps,
        &payload->leadDistance_m,
        &payload->setSpeed_mps
    );
}
