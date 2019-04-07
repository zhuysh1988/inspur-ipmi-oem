#include "bios.hpp"

#include "ipmid/utils.hpp"
#include "sys_eeprom.h"
#include "24cXX.h"

#include <ipmid/api.h>

#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <string>
#include <xyz/openbmc_project/Common/error.hpp>


static constexpr auto systemInfoServiceName = "xyz.openbmc_project.Systeminfo";
static constexpr auto systemInfoObj = "/xyz/openbmc_project/systeminfo/";
static constexpr auto systemInfoIntf = "xyz.openbmc_project.Systeminfo.Version";
static constexpr auto biosVersionProp = "BiosVersion";
static constexpr auto biosBuildTimeProp = "BiosBuildTime";

struct CPUInfoData cpu_infodata;
struct MemInfoData mem_infodata[2];
struct RevisionInfoData revision_info[2];


using namespace std;
using namespace phosphor::logging;

void register_netfn_bios_functions() __attribute__((constructor));
static uint8_t asset_info_got_flag[DEVICE_TYPE_MAX] = {0};
/* static int check_dev_present_status(uint8_t* pMaskAllPresentBuf, uint8_t MallAllPresentLen, uint8_t DeviceIndex)
{
    uint8_t unit_index = 0;
    uint8_t bit_index = 0;
    
    if(DeviceIndex >= MallAllPresentLen*8)
    {
        return ASSET_INFO_DEVICE_ABSENT;
    }
    
    unit_index = DeviceIndex/8;
    bit_index = DeviceIndex%8;
    
    if(pMaskAllPresentBuf[unit_index] & (0x01<<bit_index))
    {
        return ASSET_INFO_DEVICE_PRESENT;
    }
    else
    {
        return ASSET_INFO_DEVICE_ABSENT;
    }
    
    return ASSET_INFO_DEVICE_ABSENT;
}

static int check_dev_pconfig_status(uint8_t* pMaskAllPresentBuf, uint8_t MallAllPresentLen, uint8_t DeviceIndex)
{
    uint8_t unit_index = 0;
    uint8_t bit_index = 0;
    
    if(DeviceIndex >= MallAllPresentLen*8)
    {
        return ASSET_INFO_DEVICE_ABSENT;
    }
    
    unit_index = DeviceIndex/8;
    bit_index = DeviceIndex%8;
    
    if(pMaskAllPresentBuf[unit_index] & (0x01<<bit_index))
    {
        return ASSET_INFO_DEVICE_PRESENT;
    }
    else
    {
        return ASSET_INFO_DEVICE_ABSENT;
    }
    
    return ASSET_INFO_DEVICE_ABSENT;
}

static int check_dev_enable_status(uint8_t* pEnableStatusBuf, uint8_t EnableStatusLen, uint8_t DeviceIndex)
{
    uint8_t unit_index = 0;
    uint8_t bit_index = 0;
    
    if(DeviceIndex >= EnableStatusLen*8)
    {
        return ASSET_INFO_DEVICE_DISABLE;
    }
    
    unit_index = DeviceIndex/8;
    bit_index = DeviceIndex%8;
    
    if(pEnableStatusBuf[unit_index] & (0x01<<bit_index))
    {
        return ASSET_INFO_DEVICE_ENABLE;
    }
    else
    {
        return ASSET_INFO_DEVICE_DISABLE;
    }
    
    return ASSET_INFO_DEVICE_DISABLE;
}

static int check_dev_data_status(uint8_t* pMaskPresentBuf, uint8_t DeviceIndex)
{
    uint8_t unit_index = DeviceIndex/8;
    uint8_t bit_index = DeviceIndex%8;
    
    if(pMaskPresentBuf[unit_index] & (0x01<<bit_index))
    {
        return ASSET_INFO_DEVICE_DATA_OK;
    }
    else
    {
        return ASSET_INFO_DEVICE_DATA_NULL;
    }
    
    return ASSET_INFO_DEVICE_DATA_NULL;
} */
ipmi_ret_t update_cpu_info(SetAssetInfoFrameHeader_1B *pFrameHeader1B,uint8_t *pDeviceData)
{

    ipmi_ret_t rc = IPMI_CC_OK;
    memcpy(&cpu_infodata,pDeviceData,sizeof(CPUInfoData));
    return rc;
}

ipmi_ret_t update_mem_info(SetAssetInfoFrameHeader_1B *pFrameHeader1B,uint8_t *pDeviceData)
{

    ipmi_ret_t rc = IPMI_CC_OK;
    memcpy(&mem_infodata[0],pDeviceData,sizeof(MemInfoData));
    pDeviceData+=sizeof(MemInfoData);
    memcpy(&mem_infodata[1],pDeviceData,sizeof(MemInfoData));
    return rc;
}


ipmi_ret_t update_fw_revision(SetAssetInfoFrameHeader_1B *pFrameHeader1B,uint8_t *pDeviceData)
{



    ipmi_ret_t rc = IPMI_CC_OK;
    memcpy(&revision_info[0],pDeviceData,sizeof(RevisionInfoData));
    pDeviceData+=sizeof(RevisionInfoData);
    memcpy(&revision_info[1],pDeviceData,sizeof(RevisionInfoData));
    return rc;
}

ipmi_ret_t update_asset_info_share_memory(ipmi_request_t request)
{
    auto pFrameHeader1B = reinterpret_cast<SetAssetInfoFrameHeader_1B*>(request);
    auto pAssetInfo = reinterpret_cast<uint8_t*>(request);
    uint8_t *pDeviceData = pAssetInfo + sizeof(SetAssetInfoFrameHeader_1B);
    //uint8_t AllInfoDone =  pFrameHeader1B->AllInfoDone;
    uint8_t DeviceType = pFrameHeader1B->DeviceType;
    ipmi_ret_t rc = IPMI_CC_OK;

    switch(DeviceType)
    {
        case DEVICE_TYPE_CPU:
            {
                rc = update_cpu_info(pFrameHeader1B,pDeviceData);
                
                break;
            }
        case DEVICE_TYPE_MEM:
            {
                rc = update_mem_info(pFrameHeader1B,pDeviceData);
                break;
            }

        case DEVICE_TYPE_FW_REVISION:
            {

                rc = update_fw_revision(pFrameHeader1B,pDeviceData);
                break;
            }
        default:
            {
                //DEBUG_PRINT("[Warning]: Invalid asset device type(%d) passed into update_asset_info_share_memory() \n", pFrameHeader1B->DeviceType);
                return IPMI_CC_UNSPECIFIED_ERROR;
                break;
            }
    }
    asset_info_got_flag[DeviceType] = 1;
    return rc;
}
int prepare_eeprom(struct eeprom *e,uint8_t DeviceType,u_int8_t **data,
                int *data_len,u_int16_t *addr)
{
    std::string i2c_device = "/dev/i2c-1";
    int ret;
    ret = eeprom_open(i2c_device.data(), SYS_EEPROM_ADDRESS,   EEPROM_TYPE, e);
    if (ret != 0)
    {
        phosphor::logging::log<phosphor::logging::level::ERR>("guoms ERROR: Cannot open I2C device\n");
        return -1;
    }

    
    switch(DeviceType)
    {
        case DEVICE_TYPE_CPU:
            {
                *addr = EEPROM_OFFSET_CPU;
                *data = reinterpret_cast<u_int8_t *>(&cpu_infodata);
                *data_len = sizeof(CPUInfoData);
                break;
            }
        case DEVICE_TYPE_MEM:
            {
                *addr = EEPROM_OFFSET_MEM;
                *data = reinterpret_cast<u_int8_t *>(mem_infodata);
                *data_len = 2*sizeof(MemInfoData);
                break;
            }

        case DEVICE_TYPE_FW_REVISION:
            {
                *addr = EEPROM_OFFSET_FW_VERSION;
                *data = reinterpret_cast<u_int8_t *>(&revision_info);
                *data_len = 2*sizeof(RevisionInfoData);
                break;
            }
        default:
            {
                //DEBUG_PRINT("[Warning]: Invalid asset device type(%d) passed into update_asset_info_share_memory() \n", pFrameHeader1B->DeviceType);
                return IPMI_CC_UNSPECIFIED_ERROR;
                break;
            }
    }
    return 0;
}
int read_asset_info_from_eeprom(ipmi_request_t request,ipmi_response_t response)
{
    auto pFrameHeader1B = reinterpret_cast<SetAssetInfoFrameHeader_1B*>(request);
    uint8_t DeviceType = pFrameHeader1B->DeviceType;
    struct eeprom e;
    u_int8_t *data=NULL;
    int data_len=0;
    u_int16_t  addr = 0;
    if(prepare_eeprom(&e,DeviceType,&data,&data_len,&addr) !=0 )
    {
        return -1;
    }
    int i = 0;
    char tmp_data = 0;
    for(i=0;i<data_len;i++)
    {
        tmp_data = static_cast<char>(eeprom_read_byte(&e, addr));
        memcpy(data,&tmp_data,1);
        data++; 
        addr++;
    }
    asset_info_got_flag[DeviceType] = 1;
    return 0;

}
int write_asset_info_to_eeprom(ipmi_request_t request)
{
    auto pFrameHeader1B = reinterpret_cast<SetAssetInfoFrameHeader_1B*>(request);
    uint8_t DeviceType = pFrameHeader1B->DeviceType;

    struct eeprom e;
    u_int8_t *data=NULL;
    int data_len=0;
    u_int16_t  addr = 0;
    if(prepare_eeprom(&e,DeviceType,&data,&data_len,&addr) !=0)
    {
        return -1;
    }
    
    int i = 0;
    int ret = 0;
    for(i=0;i<data_len;i++)
    {
        ret = eeprom_write_byte(&e, addr, *data);
        if (ret < 0) 
        {
            char test[64];
            snprintf(test, sizeof(test), "write eeprom error");
            phosphor::logging::log<phosphor::logging::level::ERR>(test);
            return -1;
        }
        data++; 
        addr++;
    }
    eeprom_close(&e);
    return 0;
}

ipmi_ret_t set_system_info(ipmi_request_t request)
{
    ipmi_ret_t rc = IPMI_CC_OK;
    rc = update_asset_info_share_memory(request);
    write_asset_info_to_eeprom(request);
    return rc;
}

ipmi_ret_t ipmi_set_system_info(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
        ipmi_request_t request, ipmi_response_t response,
        ipmi_data_len_t data_len, ipmi_context_t context)
{
    ipmi_ret_t rc = IPMI_CC_OK;
    try
    {
        if (*data_len <= sizeof(SetAssetInfoFrameHeader_1B))
        {
            log<level::ERR>("Unsupported request length",
                    entry("LEN=0x%x", *data_len));
            *data_len = 0;
            return IPMI_CC_REQ_DATA_LEN_INVALID;
        }   
        rc = set_system_info(request);
    }
    catch (std::exception& e)
    {
        //log<level::ERR>(e.what());
        std::printf("bios set error: %s\n",e.what());
        return IPMI_CC_UNSPECIFIED_ERROR;
    }

    // Status code.
    *data_len = 1;
    memset(response,0x0,1);
    return rc;
}

ipmi_ret_t get_system_info(ipmi_request_t request,ipmi_response_t response, ipmi_data_len_t data_len)
{
    int offset = 0;
    auto pDataLen = reinterpret_cast<uint8_t*>(data_len);
    auto requestDate = reinterpret_cast<GetBIOSRevisionInfoData_Req*>(request);;
    uint8_t DeviceType = requestDate->DeviceType;
    ipmi_ret_t rc = IPMI_CC_OK;
    if(asset_info_got_flag[DeviceType] == 0)
    {
        if(read_asset_info_from_eeprom(request,response) != 0)
        {
            return IPMI_CC_UNSPECIFIED_ERROR;
        }
    }
    switch(DeviceType)
    {

        case DEVICE_TYPE_FW_REVISION:
            {
                
                memcpy(response, &revision_info[BIOS_INDEX], sizeof(GetBIOSRevisionInfoData_Res));
                offset += sizeof(GetBIOSRevisionInfoData_Res);
                *pDataLen = offset;
                break;
            }

        default:
            {
                return IPMI_CC_UNSPECIFIED_ERROR;
            }
    }

    return rc;
}

ipmi_ret_t ipmi_get_system_info(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
        ipmi_request_t request, ipmi_response_t response,
        ipmi_data_len_t data_len, ipmi_context_t context)
{
    // Status code.
    ipmi_ret_t rc = IPMI_CC_OK;
    try
    {    
        if (*data_len != (sizeof(GetBIOSRevisionInfoData_Req)))
        {
            log<level::ERR>("Unsupported request length",
                    entry("LEN=0x%x", *data_len));
            *data_len = 0;
            return IPMI_CC_REQ_DATA_LEN_INVALID;
        }
        rc = get_system_info(request, response, data_len);
    }
    catch (std::exception& e)
    {
        log<level::ERR>(e.what());
        return IPMI_CC_UNSPECIFIED_ERROR;
    }  
    return rc;
}

void register_netfn_bios_functions()
{
    // set bios version
    ipmiPrintAndRegister(netfunInspurAppOEM, cmdSetSystemInfo, NULL,
            ipmi_set_system_info, PRIVILEGE_ADMIN);

    //get bios version
    ipmiPrintAndRegister(netfunInspurAppOEM, cmdGetSystemInfo, NULL,
            ipmi_get_system_info, PRIVILEGE_ADMIN);

}


