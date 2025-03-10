/** Name: safetyboard.c
 *  Description: Implement the logic for system safety
 *
 */
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <stddef.h>

#include "safetyboard.h"
//x^7 + x^5 + x^3 + 1 <=> 1010101

/** CRC check based on the following
 * https://barrgroup.com/blog/crc-series-part-3-crc-implementation-code-cc
 *
 *
 */
uint8_t crc8(uint8_t* message, size_t len){
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

bool checkCRC(uint8_t CRC, uint8_t* payload, int len){
    return (crc8(payload, len) == CRC);
}

bool checkRC(uint8_t currRC, uint8_t prevRC, uint8_t RCMax){
    return (currRC == (prevRC + 1)) % RCMax;
}

bool checkCommandRange(uint8_t actReq){
    return (actReq == 0 || (actReq >= 100 && actReq <= 255));
}

uint8_t saturateOutputCommand(uint8_t actReq, uint8_t actReqPrev){
    #if DEBUG
        uint8_t actReqMax = 0;
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