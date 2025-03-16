#include <gtest/gtest.h>
#include "safetyboard.h"
#include <stdint.h>
#include <math.h>

namespace{

TEST(CRC8, TwoByteCase){
  uint8_t message1[] = {0x32,0x12};
  uint8_t crc1 = crc8(message1, sizeof(message1));
  EXPECT_EQ(crc1, 0xAD);
}

TEST(CRC8, ZeroCase){
  uint8_t message2[] = {0x00,0x00,0x00};
  uint8_t crc2 = crc8(message2, sizeof(message2));  // Poly: 0x07 (CRC-8 standard)
  EXPECT_EQ(crc2, 0x00);
}

TEST(CRC8, FourByteCase){
  uint8_t message3[] = {0x12,0x34,0x56,0x78};
  uint8_t crc3 = crc8(message3, sizeof(message3));  // Poly: 0x07 (CRC-8 standard)
  EXPECT_EQ(crc3, 0x1C);
}

TEST(CRC8, RepeatedByteCase){
  uint8_t message4[] = {0x11,0x11,0x11,0x11};
  uint8_t crc4 = crc8(message4, sizeof(message4));  // Poly: 0x07 (CRC-8 standard)
  EXPECT_EQ(crc4, 0x8D);
}

TEST(CRC8, OneByteCase){
  uint8_t message5[] = {0x31};
  uint8_t crc5 = crc8(message5, sizeof(message5));  // Poly: 0x07 (CRC-8 standard)
  EXPECT_EQ(crc5, 0x97);
}
}



