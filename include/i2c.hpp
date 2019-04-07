#pragma once

#include <ipmid/api.h>

#include <cstdint>
#include "sys_eeprom.h"

#define CPLD_VERSION_LENGTH 10
using std::uint8_t;

namespace inspur
{

/*
 * Request
 */
constexpr size_t request_rdwr = 0;
constexpr size_t request_offset = 1;
constexpr size_t request_len = 2;
// static u_int8_t eeprom[SYS_EEPROM_SIZE];

#pragma pack(push, 1)
struct GetEEPROMInfoReq
{
	uint8_t offset;
	uint8_t len;
};

struct GetEEPROMInfoRes
{
    // uint8_t completeCode;
    void*  result;
};

struct SetEEPROMInfoReq
{
	uint8_t offset;
	uint8_t len;
    uint8_t value[128];
};

struct SetMACAddressInfoReq
{
	uint8_t i2cId;
	uint8_t eepromId;
	uint8_t laddress;
	uint8_t saddress;
    uint8_t value[6];
};

struct InpurCompleteCodeRes
{
    uint8_t completeCode;
};

struct GetCPLDVerRes
{
	uint8_t version[CPLD_VERSION_LENGTH];
};

struct InputVRVerRes
{
	char bus[10];//i2c鎬荤嚎
	char addr[10];//i2c璁惧鍦板潃
	char register_addr[10];//璁惧鍐呯殑瀵勫瓨鍣ㄥ湴鍧€
};

enum adt { adt_dummy, adt_isa, adt_i2c, adt_smbus, adt_unknown };

struct adap_type {
	const char *funcs;
	const char* algo;
};

static struct adap_type adap_types[5] = {
	{ .funcs	= "dummy",
	  .algo		= "Dummy bus", },
	{ .funcs	= "isa",
	  .algo		= "ISA bus", },
	{ .funcs	= "i2c",
	  .algo		= "I2C adapter", },
	{ .funcs	= "smbus",
	  .algo		= "SMBus adapter", },
	{ .funcs	= "unknown",
	  .algo		= "N/A", },
};

struct i2c_adap {
	int nr;
	char *name;
	const char *funcs;
	const char *algo;
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


