#include "safetyboard.h"
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <string.h>

//test body
int main(){
    crcInit();

    uint8_t message0[] = {0x32,0x12};
    uint8_t crc0 = crc8(message0, sizeof(message0));  // Poly: 0x07 (CRC-8 standard)
    printf("CRC-8: 0x%02X\n", crc0);  // Print 8-bit CRC

    uint8_t message1[] = {0x32,0x12,00,00};
    uint8_t crc1 = crc8(message1, sizeof(message1));  // Poly: 0x07 (CRC-8 standard)
    printf("CRC-8: 0x%02X\n", crc1);  // Print 8-bit CRC
    
    #if DEBUG
    uint8_t padcrc0 = paddedCRC(message0, sizeof(message0)); 
    printf("PADDED CRC-8: 0x%02X\n", padcrc0);  // Print 8-bit CRC
    #endif

    uint8_t message2[] = {0x00,0x00,0x00};
    uint8_t crc2 = crc8(message2, sizeof(message2));  // Poly: 0x07 (CRC-8 standard)
    printf("CRC-8: 0x%02X\n", crc2);

    uint8_t message3[] = {0x12,0x34,0x56,0x78};
    uint8_t crc3 = crc8(message3, sizeof(message3));  // Poly: 0x07 (CRC-8 standard)
    printf("CRC-8: 0x%02X\n", crc3);

    uint8_t message4[] = {0x11,0x11,0x11,0x11};
    uint8_t crc4 = crc8(message4, sizeof(message4));  // Poly: 0x07 (CRC-8 standard)
    printf("CRC-8: 0x%02X\n", crc4);

    uint8_t message5[] = {0x31};
    uint8_t crc5 = crc8(message5, sizeof(message5));  // Poly: 0x07 (CRC-8 standard)
    printf("CRC-8: 0x%02X\n", crc5);
    
    uint8_t const message6[] = "123456789";
    uint8_t crc6 = crc8Fast(message6,sizeof(message6));
    printf("CRC-8: 0x%02X\n", crc6);

    uint8_t const message7[] = {0x12,0x34,0x56,0x78,0x09};
    uint8_t crc7 = crc8Fast(message7,sizeof(message7));
    printf("CRC-8: 0x%02X\n", crc7);

    uint8_t rolling_count = getRC();
    bool stateRC = true;
    for (int i = rolling_count; i < 300+rolling_count; i++){
        uint8_t expected = i%RCMAX;
        if (expected == getRC()){
            stateRC = (stateRC && true);
        }else{
            stateRC = false;
            break;
        }
        updateRC();
    }
    if (stateRC){
        printf("RC works!\n");
    }else{
        printf("RC doesn't work!\n");
    }

    uint8_t message[] = {0x12,0x34,0x56};
    uint8_t prevCRC = crc8(message,sizeof(message));
    for (uint8_t rc = 0; rc < 10; rc++){
        int len = sizeof(message)+1;
        uint8_t buffer[len];
        buffer[len-1] = rc;
        memcpy(&buffer[0],message,sizeof(message));

        uint8_t crc = crc8(buffer, sizeof(buffer));
        if ((crc - prevCRC) == 0){
            printf("RC messes with CRC!\n");
        }else{
            printf("RC works with CRC!\n");
        }
    }
    return 0;
}
