#pragma once
#include <cstdint>
#include <main.hpp>

namespace inspur
{
namespace ipmi
{

#pragma pack(push, 1)
struct SFPInfoReq
{
    uint8_t sfpIndex;
    uint8_t sfpStatus;
};

struct SFPStatusRes
{
    uint8_t completeCode;
    uint8_t status;
};

struct SFPEEPromRes
{
    uint8_t completeCode;
    uint8_t eeprom[maxArrayLength];
};
# pragma pack(pop)

}
}