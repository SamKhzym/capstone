#include <gtest/gtest.h>
#include "comm_layer.h"

// UNPACK TESTS

TEST(UnpackTests, EnvironmentInfoUnpackTest) {

    struct EnvironmentInfoPayload payload;
    char buffer[ENVIRONMENT_INFO_PAYLOAD_LENGTH] = "<12.345|789.012|34.567|89|102>";

    unpackEnvironmentInfoPayload(buffer, &payload);

    ASSERT_EQ(payload.leadSpeed_mps, 12.345f);
    ASSERT_EQ(payload.leadDistance_m, 789.012f);
    ASSERT_EQ(payload.setSpeed_mps, 34.567f);
    ASSERT_EQ(payload.rc, 89);
    ASSERT_EQ(payload.crc, 102);

}

TEST(UnpackTests, VehicleSpeedExtendedUnpackTest) {

    struct VehicleSpeedExtendedPayload payload;
    char buffer[VEHICLE_SPEED_EXTENDED_PAYLOAD_LENGTH] = "<34.567|1|03|045>";

    unpackVehicleSpeedExtendedPayload(buffer, &payload);

    ASSERT_EQ(payload.egoSpeed_mps, 34.567f);
    ASSERT_EQ(payload.speedFaultActive, 1);
    ASSERT_EQ(payload.rc, 3);
    ASSERT_EQ(payload.crc, 45);

}

TEST(UnpackTests, VehicleSpeedUnpackTest) {

    struct VehicleSpeedPayload payload;
    char buffer[VEHICLE_SPEED_PAYLOAD_LENGTH] = "<67.890>";

    unpackVehicleSpeedPayload(buffer, &payload);

    ASSERT_EQ(payload.vehicleSpeed_mps, 67.89f);

}

TEST(UnpackTests, SpeedCommandUnpackTest) {

    struct SpeedCommandPayload payload;
    char buffer[SPEED_COMMAND_PAYLOAD_LENGTH] = "<156>";

    unpackSpeedCommandPayload(buffer, &payload);

    ASSERT_EQ(payload.speedCommand_pwm, 156);

}

TEST(PackTests, VehicleSpeedPackTest) {

    // pack vehicle speed into bytes
    struct VehicleSpeedPayload payload = {23.456f};
    char buffer[VEHICLE_SPEED_PAYLOAD_LENGTH];

    packVehicleSpeedPayload(&payload, buffer);

    ASSERT_EQ(std::string(buffer), std::string("<23.456>"));

}

TEST(PackTests, VehicleSpeedExtendedPackTest) {

    // pack vehicle speed into bytes
    struct VehicleSpeedExtendedPayload payload = {45.654111f, 0, 3, 90};
    char buffer[VEHICLE_SPEED_EXTENDED_PAYLOAD_LENGTH];

    packVehicleSpeedExtendedPayload(&payload, buffer);

    ASSERT_EQ(std::string(buffer), std::string("<45.654|0| 3| 90>"));

}

TEST(PackTests, EnvironmentInfoPackTest) {

    // pack vehicle speed into bytes
    struct EnvironmentInfoPayload payload = {1.2346f, 9.5433f, 4.4444f, 1, 95};
    char buffer[ENVIRONMENT_INFO_PAYLOAD_LENGTH];

    packEnvironmentInfoPayload(&payload, buffer);

    ASSERT_EQ(std::string(buffer), std::string("< 1.235|  9.543| 4.444| 1| 95>"));

}

TEST(PackTests, SpeedCommandPackTest) {

    // pack vehicle speed into bytes
    struct SpeedCommandPayload payload = {255};
    char buffer[ENVIRONMENT_INFO_PAYLOAD_LENGTH];

    packSpeedCommandPayload(&payload, buffer);

    ASSERT_EQ(std::string(buffer), std::string("<255>"));

}