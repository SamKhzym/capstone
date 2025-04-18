#include <gtest/gtest.h>
#include "safetyboard.h"
#include <stdint.h>
#include <math.h>

TEST(RC1, IncrementRC){
  uint8_t rolling_count = getRC();
  for (int i = rolling_count; i < 300+rolling_count; i++){
    uint8_t expected = i%RCMAX;
    ASSERT_EQ(expected, getRC());
    updateRC();
  }
}

TEST(RC2, CRCChangesWithRC){
  uint8_t message[] = {0x12,0x34,0x56};
  uint8_t prevCRC = crc8_cap(message,sizeof(message));
  for (uint8_t rc = 0; rc < 10; rc++){
    const int len = sizeof(message)+1;
    uint8_t buffer[4];
    buffer[len-1] = rc;
    memcpy(&buffer[0],message,sizeof(message));

    uint8_t crc = crc8_cap(buffer, sizeof(buffer));
    ASSERT_NE(crc, prevCRC);
  }
}