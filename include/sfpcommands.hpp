#pragma once
#include <cstdint>
// #include <main.hpp>
// #include <types.hpp>

namespace inspur
{

void registerSFPFunctions();

static constexpr const uint8_t completeCode = 0x00;
static constexpr const uint8_t maxArrayLength = 0xFF;
static constexpr const int present_index = 192;
static constexpr const int lpmode_index = 256;
static constexpr const int interrupt_index = 224;
static constexpr const uint8_t block_base = 8;


static constexpr char eeprom_dir[] = "/sys/bus/i2c/devices";
static constexpr char eeprom_reset[] = "/sys/class/hwmon/hwmon0/device";

enum port_numbers {
qsfp1, qsfp2, qsfp3, qsfp4,
qsfp5, qsfp6, qsfp7, qsfp8,
qsfp9, qsfp10, qsfp11, qsfp12,
qsfp13, qsfp14, qsfp15, qsfp16,
qsfp17, qsfp18, qsfp19, qsfp20,
qsfp21, qsfp22, qsfp23, qsfp24,
qsfp25, qsfp26, qsfp27, qsfp28,
qsfp29, qsfp30, qsfp31, qsfp32
};

const char *path[] = {
        [qsfp1] = "6-0070/channel-0/22-0050",
        [qsfp2] = "6-0070/channel-1/23-0050",
        [qsfp3] = "6-0070/channel-2/24-0050",
        [qsfp4] = "6-0070/channel-3/25-0050",
        [qsfp5] = "6-0070/channel-4/26-0050",
        [qsfp6] = "6-0070/channel-5/27-0050",
        [qsfp7] = "6-0070/channel-6/28-0050",
        [qsfp8] = "6-0070/channel-7/29-0050",
        [qsfp9] = "6-0071/channel-0/30-0050",
        [qsfp10] = "6-0071/channel-1/31-0050",
        [qsfp11] = "6-0071/channel-2/32-0050",
        [qsfp12] = "6-0071/channel-3/33-0050",
        [qsfp13] = "6-0071/channel-4/34-0050",
        [qsfp14] = "6-0071/channel-5/35-0050",
        [qsfp15] = "6-0071/channel-6/36-0050",
        [qsfp16] = "6-0071/channel-7/37-0050",
        [qsfp17] = "6-0072/channel-0/38-0050",
        [qsfp18] = "6-0072/channel-1/39-0050",
        [qsfp19] = "6-0072/channel-2/40-0050",
        [qsfp20] = "6-0072/channel-3/41-0050",
        [qsfp21] = "6-0072/channel-4/42-0050",
        [qsfp22] = "6-0072/channel-5/43-0050",
        [qsfp23] = "6-0072/channel-6/44-0050",
        [qsfp24] = "6-0072/channel-7/45-0050",
        [qsfp25] = "6-0073/channel-0/46-0050",
        [qsfp26] = "6-0073/channel-1/47-0050",
        [qsfp27] = "6-0073/channel-2/48-0050",
        [qsfp28] = "6-0073/channel-3/49-0050",
        [qsfp29] = "6-0073/channel-4/50-0050",
        [qsfp30] = "6-0073/channel-5/51-0050",
        [qsfp31] = "6-0073/channel-6/52-0050",
        [qsfp32] = "6-0073/channel-7/53-0050",

};

const char *channel[] = {
	[qsfp1] = "6-0070/channel-0",
	[qsfp2] = "6-0070/channel-1",
	[qsfp3] = "6-0070/channel-2",
	[qsfp4] = "6-0070/channel-3",
	[qsfp5] = "6-0070/channel-4",
	[qsfp6] = "6-0070/channel-5",
	[qsfp7] = "6-0070/channel-6",
	[qsfp8] = "6-0070/channel-7",
	[qsfp9] = "6-0071/channel-0",
	[qsfp10] = "6-0071/channel-1",
	[qsfp11] = "6-0071/channel-2",
	[qsfp12] = "6-0071/channel-3",
	[qsfp13] = "6-0071/channel-4",
	[qsfp14] = "6-0071/channel-5",
	[qsfp15] = "6-0071/channel-6",
	[qsfp16] = "6-0071/channel-7",
	[qsfp17] = "6-0072/channel-0",
	[qsfp18] = "6-0072/channel-1",
	[qsfp19] = "6-0072/channel-2",
	[qsfp20] = "6-0072/channel-3",
	[qsfp21] = "6-0072/channel-4",
	[qsfp22] = "6-0072/channel-5",
	[qsfp23] = "6-0072/channel-6",
	[qsfp24] = "6-0072/channel-7",
	[qsfp25] = "6-0073/channel-0",
	[qsfp26] = "6-0073/channel-1",
	[qsfp27] = "6-0073/channel-2",
	[qsfp28] = "6-0073/channel-3",
	[qsfp29] = "6-0073/channel-4",
	[qsfp30] = "6-0073/channel-5",
	[qsfp31] = "6-0073/channel-6",
	[qsfp32] = "6-0073/channel-7",
};

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
    // uint8_t completeCode;
    uint8_t status;
};

struct SFPSumRes
{
    // uint8_t completeCode;
    uint8_t sum;
};

struct SFPEEPROMReq
{
    uint8_t sfpIndex;
    uint8_t page;
    uint8_t offset;
    uint8_t len;
};

struct SFPEEPromRes
{
    uint8_t eeprom[maxArrayLength];
};

struct InpurCompleteCodeRes
{
    uint8_t completeCode;
};
# pragma pack(pop)

}