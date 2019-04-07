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
#include <ipmid/api.h>
#include <memory>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>

#include <iostream>
#include <fstream>


#include <sys/types.h>
#include <sys/stat.h>
#include <sys/param.h>	/* for NAME_MAX */
#include <string.h>
#include <strings.h>	/* for strcasecmp() */
#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <errno.h>



namespace inspur
{

void setupGlobalInpsurI2c() __attribute__((constructor));

// Instance object.
std::unique_ptr<I2c> inspurOemI2c;

struct ParsedReq
{
	uint8_t offset;
	uint8_t len;
};

//***************vr struct start***************************************


#define MISSING_FUNC_FMT	"Error: Adapter does not have %s capability\n"

//***************vr struct end***************************************
#if SYS_EEPROM_I2C_MEM_ADDR_BITS == 8
    #define EEPROM_TYPE EEPROM_TYPE_8BIT_ADDR
#elif SYS_EEPROM_I2C_MEM_ADDR_BITS == 16
    #define EEPROM_TYPE EEPROM_TYPE_16BIT_ADDR
#else
    #define EEPROM_TYPE EEPROM_TYPE_UNKNOWN
#endif
/*
std::vector<T> ReadArray(std::ifstream &in,int size)
{
	std::vector<T> v(size,0);
	in.read((char*)(&v[0]),sizeof(T)*size);
	return v;
}*/

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
	phosphor::logging::log<phosphor::logging::level::ERR>("EEROR: open i2c device error!");
	return -1;
    }
    for (i = 0; i < len; i++) {
	*c = eeprom_read_byte(&e, addr);
	c++; addr++;
    }

    // char zys[64];
    // snprintf(zys, sizeof(zys), "zhuysh test eepromdata=%c", c);
    // phosphor::logging::log<phosphor::logging::level::ERR>(zys);

    eeprom_close(&e);
    return ret;
}


/*
 * write_sys_eeprom - write the hwinfo to i2c EEPROM
 */
// int write_sys_eeprom(void *eeprom_data, int offset, int len)
int write_sys_eeprom(void *eeprom_data, int len)
{
    int ret = 0;
    struct eeprom e;
    int i = 0;
    u_int8_t *c;
    c = static_cast<u_int8_t *>(eeprom_data);
    // u_int16_t  addr = SYS_EEPROM_OFFSET;
    u_int16_t  addr = SYS_EEPROM_OFFSET + *c;
    c++;
    int data_len = *c;
    c++;
    if (data_len != len)
    {
        return -1;
    }
    
    std::string tmp = "/dev/i2c-1";
    char* i2c_device = tmp.data();

    
    

    for (i = 0; i < len; i++) {
	if (eeprom_open(i2c_device, SYS_EEPROM_I2C_ADDR,
			EEPROM_TYPE, &e)) {
	    printf("ERROR: Cannot open I2C device\n");
        phosphor::logging::log<phosphor::logging::level::ERR>("zhuysh ERROR: Cannot open I2C device\n");
	    return -1;
	}
	int tmp = eeprom_write_byte(&e, addr, *c);
    if (tmp < 0) 
    {
        char test[64];
        snprintf(test, sizeof(test), "zhuysh test ret=%d",tmp);
        phosphor::logging::log<phosphor::logging::level::ERR>(test);
    }
	c++; addr++;
    eeprom_close(&e);
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
    // *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<GetEEPROMInfoReq*>(request);
    // GetEEPROMInfoRes getEEPROMRes{};

    char zys[64];
    snprintf(zys, sizeof(zys), "zhuysh test offset=%d, len=%d", req->offset, req->len);
    phosphor::logging::log<phosphor::logging::level::ERR>(zys);

    // u_int8_t c ;
    // int rt = read_sys_eeprom(response, req->offset, req->len);
    // getEEPROMRes.result = malloc(255);
    // if (getEEPROMRes.result == NULL)
    // {
    //     return -1;
    // }
    int rt = read_sys_eeprom(response, req->offset, req->len);

    if (rt != 0)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    // getEEPROMRes.completeCode = 0x00;
    // getEEPROMRes.result = c;
    *dataLen = req->len;

    // char test[64];
    // snprintf(test, sizeof(test), "zhuysh test lenght=%d", sizeof(c));
    // phosphor::logging::log<phosphor::logging::level::ERR>(test);
    // std::memcpy(response, &getEEPROMRes, *dataLen);
    // std::memcpy(response, &c, *dataLen);
    // free(getEEPROMRes.result);
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

    // auto req = static_cast<SetEEPROMInfoReq*>(request);

    InpurCompleteCodeRes inspurCompleteCodeRes{};


    // int rt = write_sys_eeprom(req->value, req->offset, req->len);
    int rt = write_sys_eeprom(request, *dataLen - 2);
    // int rt = write_sys_eeprom(request, *dataLen);

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

/*
 * read_cpld_eeprom - read the hwinfo from i2c CPLD
 */
int read_cpld_eeprom(void *eeprom_data, int offset, int len)
{
    int ret = 0;

	std::ifstream in("/sys/class/hwmon/hwmon0/device/version");

	in.seekg(0,in.end);

	int length = 2;
	std::printf("out length\n");
	std::printf("%d\n",length);
	in.seekg(0,in.beg);
	
	//u_int8_t *c; 
	u_int8_t data[length]={0};
	u_int8_t result = 0;	
	char* temp = new char[length];

	//c = static_cast<u_int8_t *>(eeprom_data);
	
	if(in.is_open())
	{
		std::printf("open\n");
		in.read(temp,length);
	}
	
	for(int i = 0;i < length;i++)
	{
		std::printf("start out temp\n");
		std::printf("%x\n",temp[i]);
	}
	
	for (int i=0; i<length; i++) 
	{
			
			if(temp[i]>='0' && temp[i]<='9')
				data[i] = temp[i]-'0';
			else if(temp[i]>='A' && temp[i]<='F')
				data[i] = temp[i] - 'A' + 10;
			else if(temp[i]>='a' && temp[i]<='f')
				data[i] = temp[i] - 'a' + 10;
			std::printf("%x\n",data[i]);
	}
	

	result = (data[0])*16+data[1];
	std::printf("result = %x\n",result);
	uint8_t ver1 = ((result & 0xE0) >> 5) + '0';
	uint8_t ver2 = ((result & 0x18) >> 3 )+ '0';
	uint8_t ver3 = (result & 0x07)+'0';
	//*c = (u_int8_t)*temp;
	//int c;
	GetCPLDVerRes res ;
	memset(&res,0,sizeof(res));
	res.version[0] = ver1;
	res.version[1] = '.';
	res.version[2] = ver2;
	res.version[3] = '.';
	res.version[4] = ver3;
 	
	std::printf("ver1 = %x\n",ver1);
	std::printf("ver2 = %x\n",ver2);
	std::printf("ver3 = %x\n",ver3);

	//sscanf(temp,"%x",c);
	//eeprom_data = &result;
	memcpy(eeprom_data,&res,sizeof(res));
	//*c = result;
	//std::printf("%#x\n",*(int*)eeprom_data);
	std::printf("putout\n");
	
	delete[] temp; 
	in.close();	
	return ret;
}





ipmi_ret_t ipmiReadCPLDEEPROM(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
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
    // *dataLen = 0; // default to 0 in case of an error

    auto req = static_cast<GetEEPROMInfoReq*>(request);
    // GetEEPROMInfoRes getEEPROMRes{};

    char zys[64];
    snprintf(zys, sizeof(zys), "zy test offset=%d, len=%d", req->offset, req->len);
    phosphor::logging::log<phosphor::logging::level::ERR>(zys);

    // u_int8_t c ;
    // int rt = read_sys_eeprom(response, req->offset, req->len);
    // getEEPROMRes.result = malloc(255);
    // if (getEEPROMRes.result == NULL)
    // {
    //     return -1;
    // }
    
    int rt = read_cpld_eeprom(response, req->offset, req->len);
	std::printf("from out read_icpld_eeprom");

    if (rt != 0)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    // getEEPROMRes.completeCode = 0x00;
    // getEEPROMRes.result = c;
    *dataLen = req->len;

    // char test[64];
    // snprintf(test, sizeof(test), "zhuysh test lenght=%d", sizeof(c));
    // phosphor::logging::log<phosphor::logging::level::ERR>(test);
    // std::memcpy(response, &getEEPROMRes, *dataLen);
    // std::memcpy(response, &c, *dataLen);
    // free(getEEPROMRes.result);
    return IPMI_CC_OK;
}
//**********************vr start**************************************
//************************i2cbus relate*******************************

int open_i2c_dev(int i2cbus, char *filename, size_t size, int quiet)
{
	int file;
	std::printf("inter open_i2c_dev");
	snprintf(filename, size, "/dev/i2c/%d", i2cbus);//将i2cbus号比如2放到/dev/i2c/2
	std::printf("/dev/i2c/%d\n",i2cbus);
	filename[size - 1] = '\0';
	file = open(filename, O_RDWR);

	if (file < 0 && (errno == ENOENT || errno == ENOTDIR)) {
		sprintf(filename, "/dev/i2c-%d", i2cbus);
		std::printf("/dev/i2c/%d\n",i2cbus);
		file = open(filename, O_RDWR);
	}

	if (file < 0 && !quiet) {
		if (errno == ENOENT) {
			fprintf(stderr, "Error: Could not open file "
				"`/dev/i2c-%d' or `/dev/i2c/%d': %s\n",
				i2cbus, i2cbus, strerror(ENOENT));
			std::printf("Error: Could not open file `/dev/i2c-%d' or `/dev/i2c/%d'\n",i2cbus,i2cbus);	
		} else {
			fprintf(stderr, "Error: Could not open file "
				"`%s': %s\n", filename, strerror(errno));
			std::printf("Error: Could not open file %s \n",filename);		
			if (errno == EACCES)
				fprintf(stderr, "Run as root?\n");
		}
	}
	std::printf("file = %d\n",file);
	return file;
}


static enum adt i2c_get_funcs(int i2cbus)
{
	unsigned long funcs;
	int file;
	char filename[20];
	enum adt ret;
	std::printf("inter i2c_get_funcs");
	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 1);
	if (file < 0)
		return adt_unknown;

	if (ioctl(file, I2C_FUNCS, &funcs) < 0)
	{
		ret = adt_unknown;
		std::printf("inter i2c_get_funcs error 00");
	}
	else if (funcs & I2C_FUNC_I2C)
	{
		ret = adt_i2c;
		std::printf("inter i2c_get_funcs error 01");
	}
	else if (funcs & (I2C_FUNC_SMBUS_BYTE |
			  I2C_FUNC_SMBUS_BYTE_DATA |
			  I2C_FUNC_SMBUS_WORD_DATA))
	{
		ret = adt_smbus;
		std::printf("inter i2c_get_funcs error 02");
	}
	else
	{
		ret = adt_dummy;
		std::printf("inter i2c_get_funcs error 03");
	}

	close(file);
	return ret;
}

/* Remove trailing spaces from a string
   Return the new string length including the trailing NUL */
static int rtrim(char *s)
{
	int i;

	for (i = strlen(s) - 1; i >= 0 && (s[i] == ' ' || s[i] == '\n'); i--)
		s[i] = '\0';
	std::printf("i+2 = %d",i+2);
	
	return i + 2;
}

void free_adapters(struct i2c_adap *adapters)
{
	int i;
	std::printf("inter free_adapters");
	for (i = 0; adapters[i].name; i++)
		free(adapters[i].name);
	free(adapters);
}

/* We allocate space for the adapters in bunches. The last item is a
   terminator, so here we start with room for 7 adapters, which should
   be enough in most cases. If not, we allocate more later as needed. */
#define BUNCH	8

/* n must match the size of adapters at calling time */
static struct i2c_adap *more_adapters(struct i2c_adap *adapters, int n)
{
	struct i2c_adap *new_adapters;
	std::printf("inter more_adapters");
	new_adapters = (struct i2c_adap *)realloc(adapters, (n + BUNCH) * sizeof(struct i2c_adap));
	if (!new_adapters) {
		free_adapters(adapters);
		return NULL;
	}
	memset(new_adapters + n, 0, BUNCH * sizeof(struct i2c_adap));

	return new_adapters;
}

struct i2c_adap *gather_i2c_busses(void)
{
	char s[120];
	struct dirent *de, *dde;
	DIR *dir, *ddir;
	FILE *f;
	char fstype[NAME_MAX], sysfs[NAME_MAX], n[NAME_MAX];
	int foundsysfs = 0;
	int len, count = 0;
	struct i2c_adap *adapters;
	std::printf("inter gather_i2c_busses\n");	
	adapters = (struct i2c_adap *)calloc(BUNCH, sizeof(struct i2c_adap));
	if (!adapters)
	{
		std::printf("inter gather_i2c_busses adapters error");
		return NULL;
	}

	/* look in /proc/bus/i2c */
	if ((f = fopen("/proc/bus/i2c", "r"))) {
		while (fgets(s, 120, f)) {
			char *algo, *name, *type, *all;
			int len_algo, len_name, len_type;
			int i2cbus;

			algo = strrchr(s, '\t');
			*(algo++) = '\0';
			len_algo = rtrim(algo);

			name = strrchr(s, '\t');
			*(name++) = '\0';
			len_name = rtrim(name);

			type = strrchr(s, '\t');
			*(type++) = '\0';
			len_type = rtrim(type);

			sscanf(s, "i2c-%d", &i2cbus);

			if ((count + 1) % BUNCH == 0) {
				/* We need more space */
				adapters = more_adapters(adapters, count + 1);
				if (!adapters)
					return NULL;
			}

			all =(char *) malloc(len_name + len_type + len_algo);
			if (all == NULL) {
				free_adapters(adapters);
				return NULL;
			}
			adapters[count].nr = i2cbus;
			adapters[count].name = strcpy(all, name);
			adapters[count].funcs = strcpy(all + len_name, type);
			adapters[count].algo = strcpy(all + len_name + len_type,
						      algo);
			count++;
		}
		fclose(f);
		goto done;
	}

	/* look in sysfs */
	/* First figure out where sysfs was mounted */
	if ((f = fopen("/proc/mounts", "r")) == NULL) {
		goto done;
	}
	while (fgets(n, NAME_MAX, f)) {
		sscanf(n, "%*[^ ] %[^ ] %[^ ] %*s\n", sysfs, fstype);
		if (strcasecmp(fstype, "sysfs") == 0) {
			foundsysfs++;
			break;
		}
	}
	fclose(f);
	if (! foundsysfs) {
		goto done;
	}

	/* Bus numbers in i2c-adapter don't necessarily match those in
	   i2c-dev and what we really care about are the i2c-dev numbers.
	   Unfortunately the names are harder to get in i2c-dev */
	strcat(sysfs, "/class/i2c-dev");
	if(!(dir = opendir(sysfs)))
		goto done;
	/* go through the busses */
	while ((de = readdir(dir)) != NULL) {
		if (!strcmp(de->d_name, "."))
			continue;
		if (!strcmp(de->d_name, ".."))
			continue;

		/* this should work for kernels 2.6.5 or higher and */
		/* is preferred because is unambiguous */
		len = snprintf(n, NAME_MAX, "%s/%s/name", sysfs, de->d_name);
		if (len >= NAME_MAX) {
			fprintf(stderr, "%s: path truncated\n", n);
			continue;
		}
		f = fopen(n, "r");
		/* this seems to work for ISA */
		if(f == NULL) {
			len = snprintf(n, NAME_MAX, "%s/%s/device/name", sysfs,
				       de->d_name);
			if (len >= NAME_MAX) {
				fprintf(stderr, "%s: path truncated\n", n);
				continue;
			}
			f = fopen(n, "r");
		}
		/* non-ISA is much harder */
		/* and this won't find the correct bus name if a driver
		   has more than one bus */
		if(f == NULL) {
			len = snprintf(n, NAME_MAX, "%s/%s/device", sysfs,
				       de->d_name);
			if (len >= NAME_MAX) {
				fprintf(stderr, "%s: path truncated\n", n);
				continue;
			}
			if(!(ddir = opendir(n)))
				continue;
			while ((dde = readdir(ddir)) != NULL) {
				if (!strcmp(dde->d_name, "."))
					continue;
				if (!strcmp(dde->d_name, ".."))
					continue;
				if ((!strncmp(dde->d_name, "i2c-", 4))) {
					len = snprintf(n, NAME_MAX,
						       "%s/%s/device/%s/name",
						       sysfs, de->d_name,
						       dde->d_name);
					if (len >= NAME_MAX) {
						fprintf(stderr,
							"%s: path truncated\n",
							n);
						continue;
					}
					if((f = fopen(n, "r")))
						goto found;
				}
			}
		}

found:
		if (f != NULL) {
			int i2cbus;
			enum adt type;
			char *px;

			px = fgets(s, 120, f);
			fclose(f);
			if (!px) {
				fprintf(stderr, "%s: read error\n", n);
				continue;
			}
			if ((px = strchr(s, '\n')) != NULL)
				*px = 0;
			if (!sscanf(de->d_name, "i2c-%d", &i2cbus))
				continue;
			if (!strncmp(s, "ISA ", 4)) {
				type = adt_isa;
			} else {
				/* Attempt to probe for adapter capabilities */
				type = i2c_get_funcs(i2cbus);
			}

			if ((count + 1) % BUNCH == 0) {
				/* We need more space */
				adapters = more_adapters(adapters, count + 1);
				if (!adapters)
					return NULL;
			}

			adapters[count].nr = i2cbus;
			adapters[count].name = strdup(s);
			if (adapters[count].name == NULL) {
				free_adapters(adapters);
				return NULL;
			}
			adapters[count].funcs = adap_types[type].funcs;
			adapters[count].algo = adap_types[type].algo;
			count++;
		}
	}
	closedir(dir);

done:
	return adapters;
}

static int lookup_i2c_bus_by_name(const char *bus_name)
{
	struct i2c_adap *adapters;
	int i, i2cbus = -1;
	std::printf("inter lookup_i2c_bus_by_name\n");	
	adapters = gather_i2c_busses();
	if (adapters == NULL) {
		fprintf(stderr, "Error: Out of memory!\n");
		std::printf("Error: Out of memory!\n");
		return -3;
	}

	/* Walk the list of i2c busses, looking for the one with the
	   right name */
	for (i = 0; adapters[i].name; i++) {
		if (strcmp(adapters[i].name, bus_name) == 0) {
			if (i2cbus >= 0) {
				fprintf(stderr,
					"Error: I2C bus name is not unique!\n");
				std::printf("Error: I2C bus name is not unique!\n");	
				i2cbus = -4;
				goto done;
			}
			i2cbus = adapters[i].nr;
			std::printf("inter lookup_i2c_bus_by_name for i2cbus = %d\n",i2cbus);	
		}
	}

	if (i2cbus == -1)
	{
		fprintf(stderr, "Error: I2C bus name doesn't match any "
			"bus present!\n");
		std::printf("Error: I2C bus name doesn't match any bus present!\n");	
	}
done:
	free_adapters(adapters);
	std::printf("inter lookup_i2c_bus_by_name free adapters\n");	
	return i2cbus;
}

/*
 * Parse an I2CBUS command line argument and return the corresponding
 * bus number, or a negative value if the bus is invalid.
 */
int lookup_i2c_bus(const char *i2cbus_arg)
{
	unsigned long i2cbus;
	char *end;
	std::printf("inter lookup_i2c_bus\n");	
	i2cbus = strtoul(i2cbus_arg, &end, 0);
	if (*end || !*i2cbus_arg) {
		/* Not a number, maybe a name? */
		return lookup_i2c_bus_by_name(i2cbus_arg);
	}
	if (i2cbus > 0xFFFFF) {
		fprintf(stderr, "Error: I2C bus out of range!\n");
		std::printf("Error: I2C bus out of range!\n");	
		return -2;
	}

	return i2cbus;
}

/*
 * Parse a CHIP-ADDRESS command line argument and return the corresponding
 * chip address, or a negative value if the address is invalid.
 */
int parse_i2c_address(const char *address_arg, int all_addrs)
{
	long address;
	char *end;
	long min_addr = 0x03;
	long max_addr = 0x77;
	std::printf("inter parse_i2c_address\n");	
	address = strtol(address_arg, &end, 0);
	if (*end || !*address_arg) {
		fprintf(stderr, "Error: Chip address is not a number!\n");
		std::printf("Error: Chip address is not a number!\n");	
		
		return -1;
	}

	if (all_addrs) {
		min_addr = 0x00;
		max_addr = 0x7f;
	}

	if (address < min_addr || address > max_addr) {
		fprintf(stderr, "Error: Chip address out of range "
			"(0x%02lx-0x%02lx)!\n", min_addr, max_addr);
		return -2;
	}

	return address;
}



int set_slave_addr(int file, int address, int force)
{
	/* With force, let the user read from/write to the registers
	   even when a driver is also running */
	std::printf("inter set_slave_addr\n");	   
	if (ioctl(file, force ? I2C_SLAVE_FORCE : I2C_SLAVE, address) < 0) {
		fprintf(stderr,
			"Error: Could not set address to 0x%02x: %s\n",
			address, strerror(errno));
		std::printf("Error: Could not set address to 0x%02x\n",address);		
		return -errno;
	}

	return 0;
}



/*
*i2c relate func
*/

static int check_funcs(int file, int size, int daddress, int pec)
{
	unsigned long funcs;
	std::printf("inter check_funcs\n");	

	// check adapter functionality 
	if (ioctl(file, I2C_FUNCS, &funcs) < 0) {
		fprintf(stderr, "Error: Could not get the adapter "
			"functionality matrix: %s\n", strerror(errno));
		std::printf("Error: Could not get the adapter\n");	
		return -1;
	}

	switch (size) {
	case I2C_SMBUS_BYTE:
		if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE)) {
			fprintf(stderr, MISSING_FUNC_FMT, "SMBus receive byte");
			std::printf("Error: Adapter does not have capability00\n");	
			return -1;
		}
		if (daddress >= 0
		 && !(funcs & I2C_FUNC_SMBUS_WRITE_BYTE)) {
			fprintf(stderr, MISSING_FUNC_FMT, "SMBus send byte");
			std::printf("Error: Adapter does not have capability01\n");	
			return -1;
		}
		break;

	case I2C_SMBUS_BYTE_DATA:
		if (!(funcs & I2C_FUNC_SMBUS_READ_BYTE_DATA)) {
			fprintf(stderr, MISSING_FUNC_FMT, "SMBus read byte");
			std::printf("Error: Adapter does not have capability02\n");	
			return -1;
		}
		break;

	case I2C_SMBUS_WORD_DATA:
		if (!(funcs & I2C_FUNC_SMBUS_READ_WORD_DATA)) {
			fprintf(stderr, MISSING_FUNC_FMT, "SMBus read word");
			std::printf("Error: Adapter does not have capability03\n");	
			return -1;
		}
		break;
	}

	if (pec
	 && !(funcs & (I2C_FUNC_SMBUS_PEC | I2C_FUNC_I2C))) {
		fprintf(stderr, "Warning: Adapter does "
			"not seem to support PEC\n");
		std::printf("Warning: Adapter does not seem to support PEC\n");		
	}

	return 0;
}


static int confirm(const char *filename, int address, int size, int daddress,
		   int pec)
{
	int dont = 0;
	std::printf("inter confirm\n");	
	fprintf(stderr, "WARNING! This program can confuse your I2C "
		"bus, cause data loss and worse!\n");

	//Don't let the user break his/her EEPROMs 
	if (address >= 0x50 && address <= 0x57 && pec) {
		fprintf(stderr, "STOP! EEPROMs are I2C devices, not "
			"SMBus devices. Using PEC\non I2C devices may "
			"result in unexpected results, such as\n"
			"trashing the contents of EEPROMs. We can't "
			"let you do that, sorry.\n");
		return 0;
	}

	if (size == I2C_SMBUS_BYTE && daddress >= 0 && pec) {
		fprintf(stderr, "WARNING! All I2C chips and some SMBus chips "
			"will interpret a write\nbyte command with PEC as a"
			"write byte data command, effectively writing a\n"
			"value into a register!\n");
		dont++;
	}

	fprintf(stderr, "I will read from device file %s, chip "
		"address 0x%02x, ", filename, address);
	if (daddress < 0)
		fprintf(stderr, "current data\naddress");
	else
		fprintf(stderr, "data address\n0x%02x", daddress);
	fprintf(stderr, ", using %s.\n",
		size == I2C_SMBUS_BYTE ? (daddress < 0 ?
		"read byte" : "write byte/read byte") :
		size == I2C_SMBUS_BYTE_DATA ? "read byte data" :
		"read word data");
	if (pec)
		fprintf(stderr, "PEC checking enabled.\n");
	/*
	fprintf(stderr, "Continue? [%s] ", dont ? "y/N" : "Y/n");
	fflush(stderr);
	
	if (!user_ack(!dont)) {
		fprintf(stderr, "Aborting on user request.\n");
		return 0;
	}*/

	return 1;
}
/*
 * read_vr_info - read the hwinfo from i2c vr MP2951
 */
int read_vr_info(void *eeprom_data, struct InputVRVerRes* inputversion)
{
	std::printf("into read_vr_info\n");
	char *end;

	int ret = 0 ;
	int result = 0;
	
	char filename[20];
	int  i2cbus = 0, address = 0,size = 0, file = 0,all_addrs = 0;
	int daddress;

	int pec = 0;
	int force = 0, yes = 0;



    
	std::printf("/dev/i2c-n\n");

	//i2cbus = inputversion->bus;
	i2cbus = lookup_i2c_bus(inputversion->bus);
	std::printf("i2cbus = %d\n",i2cbus);

	//address = inputversion->addr;
	address = parse_i2c_address(inputversion->addr, all_addrs);
	std::printf("address = %d\n",address);	

	//int daddress = SYS_EEPROM_OFFSET + inputversion->register_addr;
	size = I2C_SMBUS_BYTE_DATA;
	daddress = strtol(inputversion->register_addr, &end, 0);
	std::printf("daddress = %d\n",daddress);
	

	file = open_i2c_dev(i2cbus, filename, sizeof(filename), 0);
	std::printf("file = %d\n",file);

	if(file < 0)
	{
		return -1;
	}

	force = 1;
	if (file < 0
	 || check_funcs(file, size, daddress, pec)
	 || set_slave_addr(file, address, force))
		exit(1);

	if (!yes && !confirm(filename, address, size, daddress, pec))
		exit(0);

		
	std::printf("read before\n");

	
	result = vr_read_byte(file,daddress);
		
	
	if(result < 0)
	{
		std::printf("result return error\n");	
		exit(0);
	}
	
	std::printf("result=%d\n",result);
	std::printf("read after\n");

	
	uint8_t ver1 = (result >> 2) + '0';
	uint8_t ver2 = ((result &0x02)>> 1 )+ '0';
	uint8_t ver3 = (result & 0x01)+'0';
	//*c = (u_int8_t)*temp;
	//int c;
	GetCPLDVerRes res ;
	memset(&res,0,sizeof(res));
	res.version[0] = ver1;
	res.version[1] = '.';
	res.version[2] = ver2;
	res.version[3] = '.';
	res.version[4] = ver3;
 

	//sscanf(temp,"%x",c);
	//eeprom_data = &result;
	memcpy(eeprom_data,&res,sizeof(res));	
	
	close(file);

    return ret;

}

ipmi_ret_t ipmiReadVRINFO(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
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
	//--------------------------------ir3595----------------ir3584------------ir3584--------
	InputVRVerRes inputverinfo[] = {{"19","0x75","0x75"},{"18","0x50","0x89"},{"18","0x51","0x89"}};

    auto req = static_cast<GetEEPROMInfoReq*>(request);
  	
	
    /*char zys[64];
    snprintf(zys, sizeof(zys), "zy test offset=%d, len=%d", req->offset, req->len);
    phosphor::logging::log<phosphor::logging::level::ERR>(zys);*/
 
    int ret = read_vr_info(response, &inputverinfo[req->offset]);
	std::printf("from out read_vr_info");
    if (ret != 0)
    {
        return IPMI_CC_INVALID_FIELD_REQUEST;
    }

    *dataLen = req->len;

    return IPMI_CC_OK;
}

// Currently ipmid dynamically loads providers such as these;
// this creates our singleton upon load.

ipmi_ret_t ipmiSetMACToEEPROM(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                        ipmi_request_t request,
                                        ipmi_response_t response,
                                        ipmi_data_len_t dataLen,
                                        ipmi_context_t context)
{
    if (*dataLen != 10)
    {
        *dataLen = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }

	FILE* fp = NULL;
	char eerpomFileName[64] = {0};
	uint16_t offset = 0;
	
	auto req = static_cast<SetMACAddressInfoReq*>(request);

	// 这一行代码有问题
	std::sprintf(eerpomFileName, "/sys/bus/i2c/devices/%x-00%x/eeprom", req->i2cId, req->eepromId);
	// std::sprintf(eerpomFileName, "/sys/bus/i2c/devices/1-0051/eeprom");
	offset = ((uint16_t)req->laddress) << 8 | req->saddress;
	std::printf("zhuysh test: offset: %x \n", offset);

	std::printf("zhuysh test: eerpomFileName = %s \n", eerpomFileName);

	if ((fp = std::fopen(eerpomFileName, "rb+")) != NULL)
	{
		std::printf("zhuysh test: enter open\n");
		if (std::fseek(fp, offset, SEEK_SET))
		{
			phosphor::logging::log<phosphor::logging::level::ERR>("Seek eeprom file Failed!");
			std::fclose(fp);
			return IPMI_CC_UNSPECIFIED_ERROR;
		}
		std::printf("zhuysh test: seek sucess\n");
		int rt = std::fwrite(req->value, 6, 1, fp);
		if (rt != 1)
		{
			phosphor::logging::log<phosphor::logging::level::ERR>("wirte eeprom file Failed!");
			std::fclose(fp);
			return IPMI_CC_UNSPECIFIED_ERROR;
		}
		// std::fseek(fp, offset, SEEK_SET);
		std::fclose(fp);
		return IPMI_CC_OK;
	}

    return IPMI_CC_OK;
}

void setupGlobalInpsurI2c()
{
    ipmiPrintAndRegister(
        netfunInspurAppOEM,
        cmdGetEEPROMAllInfo,
        NULL, ipmiReadEEPROM, PRIVILEGE_USER);
    
    std::printf("Registering NetFn:[0x%X], Cmd:[0x%X]\n", netfunInspurAppOEM, cmdGetEEPROMAllInfo);

    ipmiPrintAndRegister(
        netfunInspurAppOEM,
        cmdsetEEPROMInfo,
        NULL, ipmiWriteEEPROM, PRIVILEGE_USER);

    std::printf("Registering NetFn:[0x%X], Cmd:[0x%X]\n", netfunInspurAppOEM, cmdsetEEPROMInfo);

	ipmiPrintAndRegister(
        netfunInspurAppOEM,
        cmdGetCPLDVersionInfo,
        NULL, ipmiReadCPLDEEPROM, PRIVILEGE_USER);

    std::printf("Registering NetFn:[0x%X], Cmd:[0x%X]\n", netfunInspurAppOEM, cmdGetCPLDVersionInfo);
	
	ipmiPrintAndRegister(
        netfunInspurAppOEM,
        cmdGetVRInfo,
        NULL, ipmiReadVRINFO, PRIVILEGE_USER);

    std::printf("Registering NetFn:[0x%X], Cmd:[0x%X]\n", netfunInspurAppOEM, cmdGetVRInfo);

	ipmiPrintAndRegister(
        netfunInspurAppOEM,
        cmdSetMACAddress,
        NULL, ipmiSetMACToEEPROM, PRIVILEGE_USER);
}
} // namespace oem
