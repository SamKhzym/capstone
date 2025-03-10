/** Name: safetyboard.h
 *  Description: header file for system safety board functionality
 *
 */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
//constants
#define POLYNOMIAL 0x07 //bin:11011000 dec:216
#define WIDTH (8*sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

#define DEBUG 1

//functions
//uint8_t crc8(uint8_t* message, size_t len, uint8_t poly, uint8_t init);
uint8_t crc8(uint8_t* message, size_t len);
bool checkCRC(uint8_t CRC, uint8_t* payload, int len);

bool checkRC(uint8_t currRC, uint8_t prevRC, uint8_t RCMax);
bool checkCommandRange(uint8_t actReq);
uint8_t saturateOutputCommand(uint8_t actReq, uint8_t actReqPrev);
bool determineState(bool crcFaultActive, bool rcFaultActive, bool wheelSpeedFaultActive, bool outOFRangeFaultActive);

void send_data(const uint8_t* data, size_t len);

