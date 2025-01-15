/** Name: safetyboard.h
 *  Description: header file for system safety board functionality
 *
 */
#include <stdint.h>
#include <stdbool.h>
//constants
#define POLYNOMIAL 0xD8 //bin:11011000 dec:216
#define WIDTH (8*sizeof(uint8_t))
#define TOPBIT (1 << (WIDTH - 1))

//functions
uint8_t checksum(uint8_t* message, int len);
bool checkCRC(uint8_t CRC, uint8_t* payload, int len);
