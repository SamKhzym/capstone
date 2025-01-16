// #pragma once

// #include <stdlib.h>
// #include "schemas.h"

// /* DEFINITION OF SCHEMA AND PAYLOAD FOR SPEED_COMMAND */
// #define NUM_SPEED_COMMAND_FIELDS 1
// // SchemaElement DEFAULT_SPEED_COMMAND_SCHEMA_ELEMENTS[NUM_SPEED_COMMAND_FIELDS] = {
// //     SchemaElement{INT, 3, 0, nullptr} // speed command
// // };

// // Schema DEFAULT_SPEED_COMMAND_SCHEMA = {
// //     .elements = DEFAULT_SPEED_COMMAND_SCHEMA_ELEMENTS,
// //     .numElements = NUM_SPEED_COMMAND_FIELDS
// // };

// struct SpeedCommandPayload {
//     int speedCommand_pwm;
// };

// /* DEFINITION OF SCHEMA AND PAYLOAD FOR VEHICLE_SPEED */
// #define NUM_VEHICLE_SPEED_FIELDS 4
// // SchemaElement DEFAULT_VEHICLE_SPEED_SCHEMA_ELEMENTS[NUM_VEHICLE_SPEED_FIELDS] = {
// //     SchemaElement{FLOAT, 6, 3, nullptr}, // ego speed
// //     SchemaElement{INT, 1, 0, nullptr}, // fault active
// //     SchemaElement{INT, 2, 0, nullptr}, // RC
// //     SchemaElement{INT, 3, 0, nullptr} // CRC
// // };

// // Schema DEFAULT_VEHICLE_SPEED_SCHEMA = {
// //     .elements = DEFAULT_VEHICLE_SPEED_SCHEMA_ELEMENTS,
// //     .numElements = NUM_VEHICLE_SPEED_FIELDS
// // };

// struct VehicleSpeedPayload {
//     float egoSpeed_mps;
//     int speedFaultActive;
//     int rc;
//     int crc;
// };

// struct SpeedCommandPayload rxSpeedCommandPayload();
// void txSpeedCommandPayload(const SpeedCommandPayload* payload);
// struct VehicleSpeedPayload rxVehicleSpeedPayload();
// void txVehicleSpeedPayload(const VehicleSpeedPayload* payload);

// // int calculateCrc(char* payload, int length);
// // int calculateRc();