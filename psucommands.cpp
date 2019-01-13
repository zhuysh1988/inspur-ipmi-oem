#include <host-ipmid/ipmid-api.h>
#include <fcntl.h>
#include <boost/container/flat_map.hpp>
#include <iostream>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/message/types.hpp>
#include <sdbusplus/timer.hpp>
#include <psucommands.hpp>
#include <main.hpp>

namespace inspur
{
namespace ipmi
{

int getPSUNum()
{
    return sizeof(psuConf)/sizeof(psuConf[0]);
}

int getGPIOValue(const int gpio)
{
    char path[64];
    char value_str[3];
    int fd;

    std::snprintf(path, sizeof(path), "/sys/class/gpio/gpio%d/value", gpio);
    fd = open(path, O_RDONLY);
    if (fd < 0)
    {
        std::fprintf(stderr, "Failed to open gpio value for reading!\n");
        return -1;
    }

    if (read(fd, value_str, 3) < 0)
    {
        std::fprintf(stderr, "Failed to read value!\n");
        return -1;
    }

    close(fd);
    return (atoi(value_str));
}

ipmi_ret_t ipmiGetPSUCount(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 1)
    {
        *dataLen = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<GetPSUNumReq*>(request);
    GetPSUNumRes psuNumRes{};

    if (req->psuCmd != psuSubCmd)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    *dataLen = sizeof(GetPSUNumRes);

    psuNumRes.completeCode = completeCode;
    psuNumRes.psuNum = getPSUNum();
    std::memcpy(response, &psuNumRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiGetPSUStatus(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 1)
    {
        *dataLen = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }

    auto req = static_cast<GetPSUStatusReq*>(request);
    GetPSUStatusRes psuInfoRes{};
    int psuIndex = req->psuIndex;
    //  判断index是否超出PSU 总数
    int psuNum = getPSUNum();
    if (psuIndex >= psuNum)
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    int psuPresent = getGPIOValue(psuConf[psuIndex]);
    if (psuPresent == -1)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    *dataLen = sizeof(GetPSUStatusRes);
    psuInfoRes.completeCode = completeCode;
    psuInfoRes.PSUInfo.Index = psuIndex;
    psuInfoRes.PSUInfo.Present = psuPresent;

    std::memcpy(response, &psuInfoRes, *dataLen);

    return IPMI_CC_OK;
}

void registerPSUFunctions()
{
    // <Get PSU number>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetPSUNumber),
        NULL, ipmiGetPSUCount, PRIVILEGE_USER);

    // <Get PSU info>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetPSUStatus),
        NULL, ipmiGetPSUStatus, PRIVILEGE_USER);
    
    return;
}
} // namespace psu
} // namespace ipmi