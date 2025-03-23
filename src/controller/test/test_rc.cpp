#include <gtest/gtest.h>
#include "safetyboard.h"
#include <stdint.h>
#include <math.h>

TEST(RC1, IncrementRC){
  uint8_t rolling_count = getRollingCount();
  for (int i = rolling_count; i < 300+rolling_count; i++){
    uint8_t expected = i%RCMAX;
    ASSERT_EQ(expected, getRollingCount);
    updateRC();
  }
}

TEST(RC2, CRCChangesWithRC){
  uint8_t message[] = {0x12,0x34,0x56};
  uint8_t prevCRC = crc8(message,sizeof(message));
  for (uint8_t rc = 0; rc < 10; rc++){
    int len = sizeof(message)+1;
    uint8_t buffer[len];
    buffer[len-1] = rc;
    memcpy(&buffer[0],message,sizeof(message));

    uint8_t crc = crc8(buffer, sizeof(buffer));
    ASSERT_NE(crc, prevCRC);
  }
}