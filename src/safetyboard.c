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
#include "safetyboard.h"
//x^7 + x^5 + x^3 + 1 <=> 1010101

/** CRC check based on the following
 * https://barrgroup.com/blog/crc-series-part-3-crc-implementation-code-cc
 *
 *
 */
uint8_t checksum(uint8_t* message, int len){
    uint8_t remainder = 0;
    for (int byte = 0; byte < len; byte++){
        remainder ^= (message[byte] << (WIDTH - 8));

        for (uint8_t bit = 8; bit > 0; bit--){
            if (remainder & TOPBIT){
                remainder = (remainder << 1) ^ POLYNOMIAL;
            }else{
                remainder = (remainder << 1);
            }
        }
    }
    return (remainder); 
}

bool checkCRC(uint8_t CRC, uint8_t* payload, int len){
    return (checksum(payload, len) == CRC);
}
