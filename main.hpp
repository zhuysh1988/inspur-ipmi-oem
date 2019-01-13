#pragma once
#include <host-ipmid/ipmid-api.h>

namespace inspur
{
namespace ipmi
{


enum IPMINetfnInspurOEMCmd
{
    // Get CPU EEPROM info.
    cmdGetEEPROMAllInfo = 0x00,
    // set CPU EEPROM info.
    cmdsetEEPROMInfo = 0x01,
    // get the number of PSUs.
    cmdGetPSUNumber = 0x2B,
    // get PSU status.
    cmdGetPSUStatus = 0x29,
    // get the number of SFPs.
    cmdGetSFPNumber = 0x40,
    // get the SFP present status.
    cmdGetSFPPrecentStatus = 0x41,
    // get the SFP low power mode.
    cmdGetSFPLowPowerMode = 0x42,
    // set SFP low power mode.
    cmdSetSFPLowPowerMode = 0x43,
    // reset SFP.
    cmdResetSFP = 0x44,
    // get SFP EEPROM info.
    cmdGetSFPEEPROMInfo = 0x45
};

enum ToolI2CCmd
{
    // 0x01.
    gpioCmd = 1,
    // 0x02.
    i2cCmd = 2,
    // 0x03.
    flashCmd = 3,
    // 0x04.
    fanManualCmd = 4,
    // 0x30.
    ethStatsCmd = 48,
    // 0x80
    blobTransferCmd = 128,
};

static constexpr ipmi_netfn_t netfunInspurAppOEM = 0x3C;
static constexpr const uint8_t maxArrayLength = 0xFF;
static constexpr const uint8_t completeCode = 0x00;


#pragma pack(push, 1)
struct InpurCompleteCodeRes
{
    uint8_t completeCode;
};
#pragma pack(pop)

} // namespace ipmi
} // namespace google
