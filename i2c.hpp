#pragma once

#include <host-ipmid/ipmid-api.h>

#include <cstdint>
#include <host-ipmid/oemrouter.hpp>

using std::uint8_t;

namespace inspur
{

/*
 * Request
 */
constexpr size_t request_rdwr = 0;
constexpr size_t request_offset = 1;
constexpr size_t request_len = 2;

#pragma pack(push, 1)
struct GetEEPROMInfoReq
{
	uint8_t offset;
	uint8_t len;
};

struct GetEEPROMInfoRes
{
    uint8_t completeCode;
    uint8_t* result;
};

struct SetEEPROMInfoReq
{
	uint8_t offset;
	uint8_t len;
};
# pragma pack(pop)

void setupGlobalInpsurI2c();

/**
 * I2c is a global i2c-via-ipmi manager and IPMI handler.
 */
class I2c
{
  public:

    /**
     * The i2c-via-ipmi commands go through this method.
     *
     * @param[in] cmd - the IPMI command.
     * @param[in] reqBuf - the IPMI command buffer.
     * @param[in,out] replyBuf - the IPMI response buffer.
     * @param[in,out] dataLen - pointer to request length, set to reply length.
     * @return IPMI return code.
     */
    ipmi_ret_t transfer(ipmi_cmd_t cmd, const uint8_t* reqBuf,
                        uint8_t* replyBuf, size_t* dataLen);
};

} // namespace oem
