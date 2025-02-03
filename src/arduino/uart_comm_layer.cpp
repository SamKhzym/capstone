#include "uart_comm_layer.h"
#include "schemas.h"
#include <Arduino.h>

// SpeedCommandPayload rxSpeedCommandPayload() {

//     SpeedCommandPayload payload;
//     Schema schema = DEFAULT_SPEED_COMMAND_SCHEMA;

//     int numBytes = schemaByteLength(&schema);

//     if (Serial.available() > 0) {

//         // allocate enough bytes for speed command payload schema
//         char* buffer = (char*)malloc(numBytes * sizeof(char));

//         // read from serial, populate schema fields
//         int length = Serial.readBytes(buffer, numBytes);
//         bytesToSchema(&schema, buffer);

//         // map schema fields to struct fields (and free memory pointers as we go)
//         payload.speedCommand_pwm = *(int*)schema.elements[0].value;
//         free(schema.elements[0].value);

//         // free the schema buffer
//         free(buffer);

//     }

//     return payload;

// }

// void txSpeedCommandPayload(const SpeedCommandPayload* payload) {

//     Schema schema = DEFAULT_SPEED_COMMAND_SCHEMA;
//     int numBytes = schemaByteLength(&schema);

//     // populate fields of schema
//     schema.elements[0].value = (void*)&(payload->speedCommand_pwm);

//     // get serialized data and write to serial
//     char* serializedData = schemaToBytes(&schema);
//     Serial.write(serializedData, numBytes);

//     free(serializedData);

// }

// struct VehicleSpeedPayload rxVehicleSpeedPayload() {

//     VehicleSpeedPayload payload;
//     Schema schema = DEFAULT_VEHICLE_SPEED_SCHEMA;

//     int numBytes = schemaByteLength(&schema);

//     if (Serial.available() > 0) {

//         // allocate enough bytes for speed command payload schema
//         char* buffer = (char*)malloc(numBytes * sizeof(char));

//         // read from serial, populate schema fields
//         int length = Serial.readBytes(buffer, numBytes);
//         bytesToSchema(&schema, buffer);

//         // map schema fields to struct fields (and free memory pointers as we go)
//         payload.egoSpeed_mps = *(float*)schema.elements[0].value;
//         payload.speedFaultActive = *(int*)schema.elements[1].value;
//         payload.rc = *(int*)schema.elements[2].value;
//         payload.crc = *(int*)schema.elements[3].value;

//         // free the individual schema value pointers and the schema buffer
//         for (int i = 0; i < schema.numElements; i++) {
//             free(schema.elements[i].value);
//         }
//         free(buffer);

//     }

//     return payload;

// }

// void txVehicleSpeedPayload(const VehicleSpeedPayload* payload) {

//     Schema schema = DEFAULT_VEHICLE_SPEED_SCHEMA;
//     int numBytes = schemaByteLength(&schema);

//     // populate fields of schema
//     schema.elements[0].value = (void*)&(payload->egoSpeed_mps);
//     schema.elements[1].value = (void*)&(payload->speedFaultActive);
//     schema.elements[2].value = (void*)&(payload->rc);
//     schema.elements[3].value = (void*)&(payload->crc);

//     // get serialized data and write to serial
//     char* serializedData = schemaToBytes(&schema);
//     Serial.write(serializedData, numBytes);

//     free(serializedData);

// }