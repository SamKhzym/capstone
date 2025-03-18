/** 
 * Name: safetyboard.c
 * Description: Implement the logic for system safety
 */

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stddef.h>
#include "safetyboard.h"

uint8_t crcTable[256];
uint8_t currRC = 0;

void crcInit(void){
    uint8_t crc;
    for (int dividend = 0; dividend < 256; dividend++){
        crc = dividend << (WIDTH - 8);
        for (uint8_t bit = 8; bit > 0; bit--){
            if (crc & TOPBIT){
                crc = (crc << 1) ^ POLYNOMIAL;
            }else{
                crc = (crc << 1);
            }
        }
        crcTable[dividend] = crc;
    }
}

uint8_t crc8Fast(uint8_t const *message, int len){
    uint8_t data;
    uint8_t crc = 0;
    for (int byte = 0; byte < len; byte++){
        data = message[byte] ^ (crc >> (WIDTH - 8));
        crc = crcTable[data] ^ (crc << 8);
    }
    return crc;
}

//use this
uint8_t crc8(uint8_t* message, int len){
    uint8_t crc = 0x00;
    for (int byte = 0; byte < len; byte++){
        crc ^= message[byte];
        for (uint8_t bit = 8; bit > 0; bit--){
            if (crc & TOPBIT){
                crc = (crc << 1) ^ POLYNOMIAL;
            }else{
                crc = (crc << 1);
            }
        }
    }
    return crc; 
}

//utility method for padding messages before doing crc
uint8_t* padMessage(uint8_t* message, size_t len){
    uint8_t* paddedMsg = malloc((len+DEGPOLY)*sizeof(uint8_t));
    for (size_t i = 0; i < len; i++){
        paddedMsg[i] = message[i];
        #if DEBUG
        printf("0x%02X 0x%02X\n",paddedMsg[i], message[i]);
        #endif
    }
    for (uint8_t i = len; i <len+DEGPOLY;  i++){
        paddedMsg[i] = 0; 
    }
    return paddedMsg;
}

/** CRC check based on the following
 * https://barrgroup.com/blog/crc-series-part-3-crc-implementation-code-cc
 * NO Reflections
 * This is WIP padding
 */
uint8_t paddedCRC(uint8_t* message, size_t len){
    message = padMessage(message,len);
    len = len+DEGPOLY;
    return crc8(message,len);
}

bool checkCRC(uint8_t CRC, uint8_t* payload, int len){
    return (crc8(payload, len-1) == CRC);// upto last byte is the original message
}

uint8_t extractCRC(uint8_t* payload, int len){
    uint8_t crc = payload[(len-1)*sizeof(uint8_t)];//24-bit starts the crc
    return crc;
}

bool checkRC(uint8_t currRC, uint8_t prevRC, uint8_t RCMax){
    return (currRC == (prevRC + 1) % RCMax);
}

// bool checkRC(uint8_t currRC, uint8_t prevRC){
//     return (currRC == (prevRC + 1) % RCMAX);
// }

void updateRC(){
    currRC = (currRC+1) % RCMAX;
}

bool checkCommandRange(uint8_t actReq){
    return (actReq == 0 || (actReq >= 100 && actReq <= 255));
}

uint8_t saturateOutputCommand(uint8_t actReq, uint8_t actReqPrev){
    #if DEBUG
        #define actReqMax 255
    #endif
    if (actReq-actReqPrev > actReqMax){
        actReq = actReq-actReqMax;
    }else if (actReqPrev - actReq > actReqMax){
        actReq = actReq + actReqMax;
    }

    if (actReq > 255){
        actReq = 255;
    }else if (actReq < 100 && actReq > 0){
        actReq = 100;
    }else if (actReq < 0){
        actReq = 0;
    }

    return actReq;
}

bool determineState(bool crcFaultActive, bool rcFaultActive, bool wheelSpeedFaultActive, bool outOFRangeFaultActive){
    return (crcFaultActive && rcFaultActive && wheelSpeedFaultActive && outOFRangeFaultActive);
}