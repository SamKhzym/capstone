#include "comm_layer.h"
#include "stdio.h"
#include <string.h>

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

void packEnvironmentInfoPayload(const struct EnvironmentInfoPayload* payload, char* payloadBuffer) {
    char leadSpeedString[6], leadDistanceString[7], setSpeedString[6];
    formatFloatString(leadSpeedString, payload->leadSpeed_mps, 6, 3);
    formatFloatString(leadDistanceString, payload->leadDistance_m, 7, 3);
    formatFloatString(setSpeedString, payload->setSpeed_mps, 6, 3);

    sprintf(payloadBuffer, "<%s|%s|%s|%2d|%3d>", 
        leadSpeedString, 
        leadDistanceString, 
        setSpeedString,
        payload->rc, 
        payload->crc
    );
}

void unpackEnvironmentInfoPayload(char* payloadBuffer, struct EnvironmentInfoPayload* payload) {
     if (payloadBuffer == NULL || payload == NULL) {
        return;
    }
    // TODO: Causes crash when uncommented
    // sscanf(payloadBuffer, "<%f|%f|%f|%d|%d>", 
    //     &payload->leadSpeed_mps,
    //     &payload->leadDistance_m,
    //     &payload->setSpeed_mps,
    //     &payload->rc,
    //     &payload->crc
    // );

    // Remove the surrounding '<' and '>'
    char *data = payloadBuffer + 1;  // Skip '<'
    data[strlen(data) - 1] = '\0';  // Remove '>'

    char *token = strtok(data, "|"); // First number
    payload->leadSpeed_mps = strtof(token, NULL);

    token = strtok(NULL, "|"); // Second number
    payload->leadDistance_m = strtof(token, NULL);

    token = strtok(NULL, "|"); // Third number
    payload->setSpeed_mps = strtof(token, NULL);
}
