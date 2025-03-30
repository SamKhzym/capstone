#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#ifdef ARDUINO
#pragma GCC diagnostic option "-Wl"
#pragma GCC diagnostic error "-u"
#pragma GCC diagnostic ignored "-lscanf_flt"
#pragma GCC diagnostic ignored "-lm"
#endif

#include <stdlib.h>

// <XXX>
#define SPEED_COMMAND_PAYLOAD_LENGTH (5+1)
struct SpeedCommandPayload {
    int speedCommand_pwm;
};
void unpackSpeedCommandPayload(const char* payloadBuffer, struct SpeedCommandPayload* payload);
void packSpeedCommandPayload(const struct SpeedCommandPayload* payload, char* payloadBuffer);

// <XX.XXX>
#define VEHICLE_SPEED_PAYLOAD_LENGTH (8+1)
struct VehicleSpeedPayload {
    float vehicleSpeed_mps;
};
void unpackVehicleSpeedPayload(const char* payloadBuffer, struct VehicleSpeedPayload* payload);
void packVehicleSpeedPayload(const struct VehicleSpeedPayload* payload, char* payloadBuffer);

// <XX.XXX|X|XX|XXX>
#define VEHICLE_SPEED_EXTENDED_PAYLOAD_LENGTH (17+1)
struct VehicleSpeedExtendedPayload {
    float egoSpeed_mps; // XX.XXX (6)
    int speedFaultActive; // X (1)
    int rc; // XX (2)
    int crc; // XXX (3)
};
void unpackVehicleSpeedExtendedPayload(const char* payloadBuffer, struct VehicleSpeedExtendedPayload* payload);
void packVehicleSpeedExtendedPayload(const struct VehicleSpeedExtendedPayload* payload, char* payloadBuffer);

// <XX.XXX|XXX.XXX|XX.XXX|XX|XXX>
#define ENVIRONMENT_INFO_PAYLOAD_LENGTH (30+1)
struct EnvironmentInfoPayload{
    float leadSpeed_mps; // XX.XXX (6)
    float leadDistance_m; // XXX.XXX (7)
    float setSpeed_mps; // XX.XXX (6)
    int rc; // XX (2)
    int crc; // XXX (3)
};
void unpackEnvironmentInfoPayload(const char* payloadBuffer, struct EnvironmentInfoPayload* payload);
void packEnvironmentInfoPayload(const struct EnvironmentInfoPayload* payload, char* payloadBuffer);

#ifdef __cplusplus
}
#endif