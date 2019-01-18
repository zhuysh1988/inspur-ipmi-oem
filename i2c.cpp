/*
 * Copyright 2018 Google Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "i2c.hpp"
#include "sys_eeprom.h"
#include "24cXX.h"

#include <fcntl.h>
#include <linux/i2c-dev.h>
#include <linux/i2c.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include <array>
#include <cerrno>
#include <cstring>
#include <host-ipmid/ipmid-api.h>
#include <memory>
#include <main.hpp>

namespace inspur
{

// Instance object.
std::unique_ptr<I2c> inspurOemI2c;

struct ParsedReq
{
	uint8_t offset;
	uint8_t len;
};

#if SYS_EEPROM_I2C_MEM_ADDR_BITS == 8
    #define EEPROM_TYPE EEPROM_TYPE_8BIT_ADDR
#elif SYS_EEPROM_I2C_MEM_ADDR_BITS == 16
    #define EEPROM_TYPE EEPROM_TYPE_16BIT_ADDR
#else
    #define EEPROM_TYPE EEPROM_TYPE_UNKNOWN
#endif

/*
 * read_sys_eeprom - read the hwinfo from i2c EEPROM
 */
int read_sys_eeprom(void *eeprom_data, int offset, int len)
{
    int ret = 0;
    struct eeprom e;
    int i = 0;
    u_int8_t *c;
    int addr = SYS_EEPROM_OFFSET + offset;
    // char *SYS_EEPROM_I2C_DEVICE = "/dev/i2c-1";
    std::string tmp = "/dev/i2c-1";
    char* i2c_device = tmp.data();
    c = static_cast<u_int8_t *>(eeprom_data);
    if (eeprom_open(i2c_device, SYS_EEPROM_I2C_ADDR,
		    EEPROM_TYPE, &e)) {
	printf("ERROR: Cannot open I2C device\n");
	return -1;
    }
    for (i = 0; i < len; i++) {
	*c = eeprom_read_byte(&e, addr);
	c++; addr++;
    }
    eeprom_close(&e);
    return ret;
}

/*
 * write_sys_eeprom - write the hwinfo to i2c EEPROM
 */
int write_sys_eeprom(void *eeprom_data, int len)
{
    int ret = 0;
    struct eeprom e;
    int i = 0;
    u_int8_t *c;
    u_int16_t  addr = SYS_EEPROM_OFFSET;
    
    std::string tmp = "/dev/i2c-1";
    char* i2c_device = tmp.data();

    c = static_cast<u_int8_t *>(eeprom_data);
    for (i = 0; i < len; i++) {
	if (eeprom_open(i2c_device, SYS_EEPROM_I2C_ADDR,
			EEPROM_TYPE, &e)) {
	    printf("ERROR: Cannot open I2C device\n");
	    return -1;
	}
	eeprom_write_byte(&e, addr, *c);
	eeprom_close(&e);
	c++; addr++;
    }

    return ret;
}

// // Parse i2c request.
// static ipmi_ret_t parse(const uint8_t* reqBuf, size_t reqLen, i2c::ParsedReq* req)
// {
// 	if (reqLen != 2 && reqLen != 3)
//     {
//         std::fprintf(stderr, "i2c::parse reqLen=%zu?\n", reqLen);
//         return IPMI_CC_REQ_DATA_LEN_INVALID;
//     }
// 	if (reqLen == 3)
// 		req->offset = reqBuf[request_offset];
	
// 	req->rdwr = reqBuf[request_rdwr];
// 	req->len = reqBuf[request_len];

// 	return IPMI_CC_OK;
// }

ipmi_ret_t ipmiReadEEPROM(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 2)
    {
        *dataLen = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<GetEEPROMInfoReq*>(request);
    GetEEPROMInfoRes getEEPROMRes{};

    int rt = read_sys_eeprom(getEEPROMRes.result, req->offset, req->len);

    if (rt != 0)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    getEEPROMRes.completeCode = completeCode;
    *dataLen = sizeof(getEEPROMRes);

    std::memcpy(response, &getEEPROMRes, *dataLen);

    return IPMI_CC_OK;
}

ipmi_ret_t ipmiWriteEEPROM(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen == 0)
    {
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }

    InpurCompleteCodeRes inspurCompleteCodeRes{};

    int rt = write_sys_eeprom(request, *dataLen);

    if (rt != 0)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    *dataLen = sizeof(inspurCompleteCodeRes);

    std::memcpy(response, &inspurCompleteCodeRes, *dataLen);

    return IPMI_CC_OK;
}



// /**
//  * read:  ipmitool raw 0x2e 2 read(0) offset len 
//  * write: ipmitool raw 0x2e 2 write(1) len
//  */
// ipmi_ret_t I2c::transfer(ipmi_cmd_t cmd, const uint8_t* reqBuf, uint8_t* replyBuf, size_t* dataLen)
// {
//     // Parse message header.
//     auto reqLen = *dataLen;
//     *dataLen = 0;
//     i2c::ParsedReq req = {};
//     auto rc = parse(reqBuf, reqLen, &req);
//     if (rc != IPMI_CC_OK)
//     {
//         return rc;
//     }

// 	if (req.rdwr == 0)	/* replyBuf��������buffer??? */
// 		read_sys_eeprom(replyBuf, req.offset, req.len);	/* uint8_t -> int */
// 	else
// 		write_sys_eeprom(replyBuf, req.len);
	
// 	*dataLen = req.len;	/* ��ʱֱ�ӷ��� */
		
//     return IPMI_CC_OK;
// }

// ipmi_ret_t registerI2COemCmd(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
//                                         ipmi_request_t request,
//                                         ipmi_response_t response,
//                                         ipmi_data_len_t dataLen,
//                                         ipmi_context_t context)
// {
//     const uint8_t* reqBuf = static_cast<uint8_t*>(request);
//     uint8_t* replyBuf = static_cast<uint8_t*>(response);

//     inspur::i2c::inspurOemI2c = std::make_unique<I2c>();
//     return inspur::i2c::inspurOemI2c->transfer(cmd, reqBuf, replyBuf, dataLen);
// }



// Currently ipmid dynamically loads providers such as these;
// this creates our singleton upon load.

void setupGlobalInpsurI2c()
{
    ipmi_register_callback(
        netfunInspurAppOEM,
        cmdGetEEPROMAllInfo,
        NULL, ipmiReadEEPROM, PRIVILEGE_USER);
    
    ipmi_register_callback(
        netfunInspurAppOEM,
        cmdsetEEPROMInfo,
        NULL, ipmiReadEEPROM, PRIVILEGE_USER);
}
} // namespace oem
