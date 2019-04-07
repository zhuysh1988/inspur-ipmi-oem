
#pragma once
#include <ipmid/api.h>
#include <cstddef>
#include <stdint.h>

#define ALL_INFO_NOT_DONE       (0)
#define ALL_INFO_DONE           (1)

#define BIOS_INFO_EEPROM_START_ADDR     16*1024
#define BIOS_INFO_EEPROM_END_ADDR       32*1024

#define EEPROM_PAGE_SIZE                    (0x40)    //64B
#define ASSET_INFO_CPU_INFO_SPACE           (2 * EEPROM_PAGE_SIZE)  //1 *89B=89B=2*64B =>2*64B=>2Pages
#define ASSET_INFO_MEM_INFO_SPACE           (2 * EEPROM_PAGE_SIZE)  //4*25B=100B=2*64B =>2*64B=> 2Pages
#define ASSET_INFO_FW_VERSION_INFO_SPACE    (2 * EEPROM_PAGE_SIZE) //2*50B=100B= 2*64B =>2*64B=> 2Pages

#define EEPROM_OFFSET_CPU               (BIOS_INFO_EEPROM_START_ADDR)
#define EEPROM_OFFSET_MEM               (EEPROM_OFFSET_CPU + ASSET_INFO_CPU_INFO_SPACE)
#define EEPROM_OFFSET_FW_VERSION        (EEPROM_OFFSET_MEM + ASSET_INFO_MEM_INFO_SPACE)

#define ME_INDEX 0
#define BIOS_INDEX 1


#define SYS_EEPROM_ADDRESS      0x50
#define EEPROM_TYPE EEPROM_TYPE_16BIT_ADDR

//asset information, device type define
typedef enum
{
    DEVICE_TYPE_NONE=0,
    DEVICE_TYPE_CPU,                    //1-CPU
    DEVICE_TYPE_MEM,                    //2-MEM
    DEVICE_TYPE_FW_REVISION=0x7,            //7-BIOS/ME/IE/PCH SKU Version

}SYS_DEVICE_TYPE;
static constexpr auto DEVICE_TYPE_MAX = 8;
static constexpr size_t maxVersionStrLen = 15;
static constexpr size_t maxBuildTimeStrLen = 20;
static constexpr auto NUMBER_OF_REVISION = 8;

static constexpr auto ASSET_INFO_DEVICE_ABSENT = 0;
static constexpr auto ASSET_INFO_DEVICE_PRESENT = 1;

static constexpr auto ASSET_INFO_CONFIG_ABSENT = 0;
static constexpr auto ASSET_INFO_CONFIG_PRESENT = 1;

static constexpr auto ASSET_INFO_DEVICE_DISABLE = 0;
static constexpr auto ASSET_INFO_DEVICE_ENABLE = 1;

static constexpr auto ASSET_INFO_DEVICE_DATA_NULL = 0;
static constexpr auto ASSET_INFO_DEVICE_DATA_OK = 1;

static constexpr auto REVISION_DEVICE_NAME_LEN = 15;
static constexpr auto REVISION_DEVICE_VERSION_LEN = 15;
static constexpr auto REVISION_BUILD_TIME_LEN = 20;

static constexpr auto PROCESSOR_NAME_LEN = 50;
static constexpr auto SIGNATURE_LEN = 8;
static constexpr auto MICROCODE_LEN = 10;
static constexpr auto MAXSPEED_LEN = 10;

static constexpr auto MANUFACTURE_LEN = 10;
static constexpr auto SN_NUMBER_LEN = 10;
//asset information frame header define
struct SetAssetInfoFrameHeader_1B
{
    //uint8_t  RWFlag;
    uint8_t  DeviceType;            //device type
    //uint8_t  InfoType;            //00:common info, 01:private(different) info
    //uint8_t  MaskAllPresentLen;   //device bit space, =1B
    //uint8_t  EnableStatus[1];     //0:Disable, 1:Enable
    uint8_t  MaskPresent[4];        //0:Absent,  1:Present
    uint8_t  MaskAllPresent[4]; //0:Absent,  1:Present;用于表示Config
    uint8_t  AllInfoDone;           //0:not done, 1:done
    uint8_t  TotalMessageLen[4];        //total message length
};

struct CPUInfoData
{
    uint8_t Present;
    uint8_t ProcessorName[PROCESSOR_NAME_LEN];
    uint8_t Signature[SIGNATURE_LEN];
    uint8_t MicroCode[MICROCODE_LEN];
    uint8_t Maxspeed[MAXSPEED_LEN];
    uint8_t Core;
    uint8_t UsedCore;
    uint16_t L1_Data;
    uint16_t L2_Cache;
    uint16_t L3_Cache;
    uint16_t CPUTDP;

};

struct MemInfoData
{
    uint8_t Status;
    uint8_t Manufacture[MANUFACTURE_LEN];
    uint8_t SN_Number[SN_NUMBER_LEN];
    uint16_t Capacity;
    uint16_t Speed;

};

struct RevisionInfoData
{
    uint8_t  DevName[REVISION_DEVICE_NAME_LEN];
    uint8_t  DevRev[REVISION_DEVICE_VERSION_LEN];
    uint8_t  BuildTime[REVISION_BUILD_TIME_LEN];
};


struct GetBIOSRevisionInfoData_Req
{
    uint8_t  DeviceType;
};
struct GetBIOSRevisionInfoData_Res
{
    uint8_t  DevName[REVISION_DEVICE_NAME_LEN] ;
    uint8_t  DevRev[REVISION_DEVICE_VERSION_LEN];
    uint8_t  BuildTime[REVISION_BUILD_TIME_LEN];
};


