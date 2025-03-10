#include "safetyboard.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

//test body
int main(){
    uint8_t message[] = {0x32,0x12};
    uint8_t crc = crc8(message, sizeof(message));  // Poly: 0x07 (CRC-8 standard)
    
    printf("CRC-8: 0x%02X\n", crc);  // Print 8-bit CRC
    return 0;
}
