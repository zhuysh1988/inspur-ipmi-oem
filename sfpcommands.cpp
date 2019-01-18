#include <host-ipmid/ipmid-api.h>
#include <utils.hpp>
#include <fcntl.h>
#include <boost/container/flat_map.hpp>
#include <iostream>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/message/types.hpp>
#include <sdbusplus/bus.hpp>
#include <sfpcommands.hpp>
#include <main.hpp>

namespace inspur
{

constexpr auto sfp_service = "xyz.inspur.sfp";
constexpr auto sfp_object = "/xyz/inspur/sfp";
constexpr auto sfp_interface = "xyz.inspur.sfp";
constexpr auto sfp_lowpower_property = "LowPowerMode";
constexpr auto sfp_reset_method = "Reset";

bool getSFPLowPowerMode(int sfp_id)
{
    sdbusplus::bus::bus bus{ipmid_get_sd_bus_connection()};

    ipmi::DbusObjectInfo sfpObject = ipmi::getDbusObject(bus, sfp_interface);
    auto variant = ipmi::getDbusProperty(bus, sfpObject.second, sfpObject.first, sfp_interface, sfp_lowpower_property);

    return sdbusplus::message::variant_ns::get<bool>(variant);
    // Value value;
    // auto bus = sdbusplus::bus::new_default();
    // auto method = bus.new_method_call(sfp_service, sfp_object, PROP_INTF, METHOD_GET);
    // method.append(sfp_interface, sfp_lowpower_property);

    // auto reply = bus.call(method);

    // if (reply.is_method_error())
    // {
    //     std::fprintf(stderr, "Failed to call dbus method!\n");
    // }
    // reply.read(value);
    // return variant_ns::get<bool>(value);
}

int getSFPSum()
{
    int sum = 32;
    return sum;
}

bool getSFPPresent(int sfp_id)
{
    return true;
}

ipmi_ret_t ipmiGetSFPPresent(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
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

    auto req = static_cast<SFPInfoReq*>(request);
    SFPStatusRes sfpStatusRes{};

    if (req->sfpIndex >= getSFPSum())
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    *dataLen = sizeof(SFPStatusRes);

    sfpStatusRes.completeCode = completeCode;
    sfpStatusRes.status = getSFPPresent(req->sfpIndex) ? 0x01 : 0x00;
    std::memcpy(response, &sfpStatusRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiGetSFPLowPowerMode(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
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

    auto req = static_cast<SFPInfoReq*>(request);
    SFPStatusRes sfpStatusRes{};

    if (req->sfpIndex >= getSFPSum())
    {
        return IPMI_CC_PARM_OUT_OF_RANGE;
    }

    *dataLen = sizeof(SFPStatusRes);

    sfpStatusRes.completeCode = completeCode;
    sfpStatusRes.status = getSFPLowPowerMode(req->sfpIndex) ? 0x01 : 0x00;
    std::memcpy(response, &sfpStatusRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiGetSFPSum(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 0)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    SFPSumRes sfpSumRes{};

    *dataLen = sizeof(SFPSumRes);

    sfpSumRes.completeCode = completeCode;
    sfpSumRes.sum = getSFPSum();
    std::memcpy(response, &sfpSumRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiSFPReset(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 0)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    SFPSumRes sfpSumRes{};

    *dataLen = sizeof(SFPSumRes);

    sfpSumRes.completeCode = completeCode;
    sfpSumRes.sum = getSFPSum();
    std::memcpy(response, &sfpSumRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiSetSFPLowPowerMode(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 0)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    SFPSumRes sfpSumRes{};

    *dataLen = sizeof(SFPSumRes);

    sfpSumRes.completeCode = completeCode;
    sfpSumRes.sum = getSFPSum();
    std::memcpy(response, &sfpSumRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmigetSFPEEPROM(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 0)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    SFPSumRes sfpSumRes{};

    *dataLen = sizeof(SFPSumRes);

    sfpSumRes.completeCode = completeCode;
    sfpSumRes.sum = getSFPSum();
    std::memcpy(response, &sfpSumRes, *dataLen);

    return IPMI_CC_OK;
}

void registerSFPFunctions()
{
    // <Get SFP number>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPNumber),
        NULL, ipmiGetSFPSum, PRIVILEGE_USER);

    // <Get SFP present>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPPrecentStatus),
        NULL, ipmiGetSFPPresent, PRIVILEGE_USER);

    // <Get SFP low power mode>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPLowPowerMode),
        NULL, ipmiGetSFPLowPowerMode, PRIVILEGE_USER);
    
    // <set SFP low power mode>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdSetSFPLowPowerMode),
        NULL, ipmiSetSFPLowPowerMode, PRIVILEGE_USER);

    // <Rest SFP>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdResetSFP),
        NULL, ipmiSFPReset, PRIVILEGE_USER);

    // <Get SFP EEPROM>
    ipmi_register_callback(
        netfunInspurAppOEM,
        static_cast<ipmi_cmd_t>(IPMINetfnInspurOEMCmd::cmdGetSFPEEPROMInfo),
        NULL, ipmigetSFPEEPROM, PRIVILEGE_USER);

    return;
}
} // namespace ipmi