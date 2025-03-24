#include "comm_layer.h"
#include "stdio.h"
#include <string.h>

void formatFloatString(char* buffer, float value, int length, int numDecimals) {
    snprintf(buffer, length+1, "%*.*f", length, numDecimals, value);
}

void packSpeedCommandPayload(const struct SpeedCommandPayload* payload, char* payloadBuffer) {
    sprintf(payloadBuffer, "<%3d>", payload->speedCommand_pwm);
}

void unpackSpeedCommandPayload(const char* payloadBuffer, struct SpeedCommandPayload* payload) {
    if (payloadBuffer == NULL || payload == NULL) {
        return;
    }

    // Remove the surrounding '<' and '>'
    char *data = payloadBuffer + 1;  // Skip '<'
    data[strlen(data) - 1] = '\0';  // Remove '>'

    char *token = strtok(data, "|"); // First number
    payload->speedCommand_pwm = atoi(token);
}

void packVehicleSpeedPayload(const struct VehicleSpeedPayload* payload, char* payloadBuffer) {
    char speedString[10];
    formatFloatString(speedString, payload->vehicleSpeed_mps, 6, 3);
    sprintf(payloadBuffer, "<%s>", speedString);
}

void unpackVehicleSpeedPayload(const char* payloadBuffer, struct VehicleSpeedPayload* payload) {
    if (payloadBuffer == NULL || payload == NULL) {
        return;
    }

    // Remove the surrounding '<' and '>'
    char *data = payloadBuffer + 1;  // Skip '<'
    data[strlen(data) - 1] = '\0';  // Remove '>'

    char *token = strtok(data, "|"); // First number
    payload->vehicleSpeed_mps = strtof(token, NULL);

}

void packVehicleSpeedExtendedPayload(const struct VehicleSpeedExtendedPayload* payload, char* payloadBuffer) {
    char speedString[7];
    formatFloatString(speedString, payload->egoSpeed_mps, 6, 3);
    
    sprintf(payloadBuffer, "<%s|%1d|%2d|%3d>", speedString, payload->speedFaultActive, payload->rc, payload->crc);
}

void unpackVehicleSpeedExtendedPayload(const char* payloadBuffer, struct VehicleSpeedExtendedPayload* payload) {
    if (payloadBuffer == NULL || payload == NULL) {
        return;
    }

    // Remove the surrounding '<' and '>'
    char *data = payloadBuffer + 1;  // Skip '<'
    data[strlen(data) - 1] = '\0';  // Remove '>'

    char *token = strtok(data, "|"); // First number
    payload->egoSpeed_mps = strtof(token, NULL);

    token = strtok(NULL, "|"); // Second number
    payload->speedFaultActive = atoi(token);
    
    token = strtok(NULL, "|"); // Fourth number
    payload->rc = atoi(token);
    
    token = strtok(NULL, "|"); // Fifth number
    payload->crc = atoi(token);

}

void packEnvironmentInfoPayload(const struct EnvironmentInfoPayload* payload, char* payloadBuffer) {
    char leadSpeedString[10], leadDistanceString[10], setSpeedString[10];
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

void unpackEnvironmentInfoPayload(const char* payloadBuffer, struct EnvironmentInfoPayload* payload) {
     if (payloadBuffer == NULL || payload == NULL) {
        return;
    }

    // Remove the surrounding '<' and '>'
    char *data = payloadBuffer + 1;  // Skip '<'
    data[strlen(data) - 1] = '\0';  // Remove '>'

    char *token = strtok(data, "|"); // First number
    payload->leadSpeed_mps = strtof(token, NULL);

    token = strtok(NULL, "|"); // Second number
    payload->leadDistance_m = strtof(token, NULL);

    token = strtok(NULL, "|"); // Third number
    payload->setSpeed_mps = strtof(token, NULL);
    
    token = strtok(NULL, "|"); // Fourth number
    payload->rc = atoi(token);
    
    token = strtok(NULL, "|"); // Fifth number
    payload->crc = atoi(token);

}
