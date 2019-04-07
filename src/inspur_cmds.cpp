#include <ipmid/api.h>
#include "inspur_cmds.hpp"
#include <string>
#include <stdio.h>
#include<iostream>
#include<fstream>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/message/types.hpp>
#include <set>
#include <xyz/openbmc_project/Common/error.hpp>
#include <xyz/openbmc_project/Sensor/Value/server.hpp>
#include "ipmid/utils.hpp"
using namespace phosphor::logging;
using InternalFailure =
    sdbusplus::xyz::openbmc_project::Common::Error::InternalFailure;
using namespace std;
void register_netfn_inspur_cmds_functions() __attribute__((constructor));
extern const ipmi::sensor::IdInfoMap sensors;

FILE *fp = NULL;
uint8_t getFanMode()
{
    string get_fan_mode_cmd = "cat /usr/bin/fan_mode";
    char buffer[10] = {0};
    uint8_t rc = 1;
    fp = popen(get_fan_mode_cmd.c_str(),"r");
    if(fp != NULL)
    {
        fgets(buffer,10,fp);
        pclose(fp);
        fp = NULL;
    }
    rc = buffer[0]-'0';
    return rc;
}
uint8_t setFanMode(uint8_t mode)
{
    char set_fan_mode_cmd[100]={0};
    uint8_t rc = 1;
    sprintf(set_fan_mode_cmd,"echo %d > /usr/bin/fan_mode",mode);
    rc = system(set_fan_mode_cmd);
    return rc;
}


ipmi_ret_t setFan(void *request)
{
    ipmi::sensor::SetSensorReadingReq cmdData ;
    memset(&cmdData,0x00,sizeof(ipmi::sensor::SetSensorReadingReq));
    auto reqptr = static_cast<SetFanReq*>(request);
    cmdData.number = reqptr->sensor_num;
    cmdData.reading = reqptr->value;
    
    // Check if the Sensor Number is present
    const auto iter = sensors.find(cmdData.number);
    if (iter == sensors.end())
    {
        return IPMI_CC_SENSOR_INVALID;
    }
    
    try
    {   
        if(setFanMode(reqptr->mode) != 0)
        {
            return IPMI_CC_UNSPECIFIED_ERROR;
        }
        //set fan speed when mode is manual
        if(reqptr->mode != auto_mode)
        {
            const auto iter = sensors.find(cmdData.number);
            if (iter == sensors.end())
            {
                return IPMI_CC_SENSOR_INVALID;
            }

            if (ipmi::sensor::Mutability::Write !=
                (iter->second.mutability & ipmi::sensor::Mutability::Write))
            {
                log<level::ERR>("Sensor Set operation is not allowed",
                                entry("SENSOR_NUM=%d", cmdData.number));
                return IPMI_CC_ILLEGAL_COMMAND;
            }
            if(iter->second.sensorType == fan_type &&  
                iter->second.propertyInterfaces.begin()->first != ipmiValueInf)
            {
                return iter->second.updateFunc(cmdData, iter->second);
            }
            else
            {
                return IPMI_CC_ILLEGAL_COMMAND;
            }
        }
        return IPMI_CC_OK;
    }
    catch (InternalFailure& e)
    {
        log<level::ERR>("Set sensor failed",
                        entry("SENSOR_NUM=%d", cmdData.number));
        commit<InternalFailure>();
    }
    catch (const std::runtime_error& e)
    {
        log<level::ERR>(e.what());
    }

    return IPMI_CC_UNSPECIFIED_ERROR;

}
ipmi_ret_t getFan(void *request,void *response)
{
    auto reqptr = static_cast<GetFanReq*>(request);
    auto resptr = static_cast<GetFanRes*>(response);
    const auto iter = sensors.find(reqptr->sensor_num);
    ipmi::sensor::GetSensorResponse getResponse{};
    if (iter == sensors.end())
    {
        return IPMI_CC_SENSOR_INVALID;
    }
    if (ipmi::sensor::Mutability::Read !=
        (iter->second.mutability & ipmi::sensor::Mutability::Read))
    {
        return IPMI_CC_ILLEGAL_COMMAND;
    }
    if(iter->second.sensorType != fan_type)
    {
        return IPMI_CC_SENSOR_INVALID;
    }
    try
    {
        getResponse = iter->second.getFunc(iter->second);
        resptr->mode = getFanMode();
        resptr->value = getResponse[0];
        return IPMI_CC_OK;
    }
    catch (const std::exception& e)
    {

        log<level::ERR>(e.what());
        return IPMI_CC_UNSPECIFIED_ERROR;
    }
}

ipmi_ret_t ipmi_set_fan(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                    ipmi_request_t request, ipmi_response_t response,
                                    ipmi_data_len_t data_len, ipmi_context_t context)
{
    auto reqptr = static_cast<SetFanReq*>(request);
    if((reqptr->mode >> 1) != 0x0)
    {
        *data_len = 0;
        return IPMI_CC_ILLEGAL_COMMAND;
    }
    if(reqptr->mode == auto_mode)
    {

        if (*data_len != sizeof(SetFanReq)-1)
        {
            *data_len = 0;
            return IPMI_CC_ILLEGAL_COMMAND;
        }
    }
    else
    {
      if(*data_len != sizeof(SetFanReq))
      {
        *data_len = 0;
        return IPMI_CC_ILLEGAL_COMMAND;
      }
    } 

    auto rc = setFan(request);
    *data_len = 0;
    return rc;
 
}
ipmi_ret_t ipmi_get_fan(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                    ipmi_request_t request, ipmi_response_t response,
                                    ipmi_data_len_t data_len, ipmi_context_t context)
{
    if(*data_len != sizeof(GetFanReq) )
    {
        *data_len = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    
    auto rc = getFan(request,response);
    if(rc == IPMI_CC_OK)
    {
        *data_len = sizeof(GetFanRes);
    }
    else
    {
        *data_len = 0;
    }
    return rc;
 
}


void register_netfn_inspur_cmds_functions()
{
    // get bmc version info
    ipmiPrintAndRegister(netfunInspurAppOEM, cmdSetFan, NULL,
                           ipmi_set_fan, PRIVILEGE_ADMIN);

    ipmiPrintAndRegister(netfunInspurAppOEM, cmdGetFan, NULL,
                           ipmi_get_fan, PRIVILEGE_ADMIN);
}

