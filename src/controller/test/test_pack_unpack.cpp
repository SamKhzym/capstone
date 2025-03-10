#include <gtest/gtest.h>
#include "comm_layer.h"

// TEST(UnpackTests, EnvironmentInfoPackTest) {

//     struct EnvironmentInfoPayload payload;
//     char buffer[ENVIRONMENT_INFO_PAYLOAD_LENGTH] = "<12.345|789.012|34.567|89|102>";

//     unpackEnvironmentInfoPayload(buffer, &payload);

//     ASSERT_EQ(payload.leadSpeed_mps, 12.345f);
//     ASSERT_EQ(payload.leadDistance_m, 789.012f);
//     ASSERT_EQ(payload.setSpeed_mps, 34.567f);
//     ASSERT_EQ(payload.rc, 89.0f);
//     ASSERT_EQ(payload.crc, 102.0f);

// }

TEST(PackTests, VehSpeedPackTest) {

    // pack vehicle speed into bytes
    struct VehicleSpeedPayload payload = {23.456f};
    char buffer[VEHICLE_SPEED_PAYLOAD_LENGTH];

    packVehicleSpeedPayload(&payload, buffer);

    ASSERT_EQ(std::string(buffer), std::string("<23.456>"));

}