/** Name: safetyboard.h
 *  Description: header file for system safety board functionality
 *
 */

#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
//constants
#define POLYNOMIAL 0x07 //bin:0000 0111 dec:7 Polynomial: x^2 + x^1 + x^0
#define DEGPOLY 2
#define WIDTH (8*sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))
#define NUMBITSMSG 32 //Bits Per Message
#define RCMAX 256 //rolling count

#define DEBUG 1

//functions
void crcInit(void);
uint8_t crc8Fast(uint8_t const *message, int len);

uint8_t crc8_cap(uint8_t* message, int len);
uint8_t paddedCRC(uint8_t* message, size_t len);

bool checkCRC(uint8_t CRC, uint8_t* payload, int len);
uint8_t extractCRC(uint8_t* payload, int len);

bool checkRC(uint8_t currRC, uint8_t prevRC, uint8_t RCMax);
void updateRC(void);
uint8_t getRC(void);

bool checkCommandRange(uint8_t actReq);
uint8_t saturateOutputCommand(uint8_t actReq, uint8_t actReqPrev);
bool determineState(bool crcFaultActive, bool rcFaultActive, bool wheelSpeedFaultActive, bool outOFRangeFaultActive);

void send_data(const uint8_t* data, size_t len);

#ifdef __cplusplus
}
#endif