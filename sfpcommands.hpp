#pragma once
#include <cstdint>
#include <main.hpp>
#include <types.hpp>

namespace inspur
{

#pragma pack(push, 1)
struct SFPInfoReq
{
    uint8_t sfpIndex;
};

struct SFPSetModeReq
{
    uint8_t sfpIndex;
    uint8_t lowMode;
};

struct SFPStatusRes
{
    uint8_t completeCode;
    uint8_t status;
};

struct SFPSumRes
{
    uint8_t completeCode;
    uint8_t sum;
};

struct SFPEEPromRes
{
    uint8_t completeCode;
    uint8_t eeprom[maxArrayLength];
};
# pragma pack(pop)

}