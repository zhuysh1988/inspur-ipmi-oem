#pragma once
#include <cstdint>
#include <string>
#define BMC_VERSION_LENGTH 40
#define BMC_BUILDTIME_LENGTH 20
static constexpr uint8_t fan_type = 0x04;
static constexpr uint8_t auto_mode = 0x01;
static const std::string ipmiValueInf = "xyz.openbmc_project.Sensor.Value";
struct SetFanReq
{
    uint8_t sensor_num;
    uint8_t mode;
    uint8_t value;
};

struct GetFanReq
{
    uint8_t sensor_num;
};
struct GetFanRes
{
    uint8_t mode;
    uint8_t value;
};
