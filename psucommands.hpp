#pragma once
#include <cstdint>

namespace inspur
{

// PSU GPIO configure
static constexpr const int psuConf[2] = {320, 321};

static constexpr const uint8_t psuSubCmd = 0x00;
static constexpr const uint8_t PSU_MFR_LEN			(16);
static constexpr const uint8_t PSU_MODEL_LEN		(16);
static constexpr const uint8_t PSU_SN_LEN			(16);
static constexpr const uint8_t PSU_PN_LEN			(16);
static constexpr const uint8_t PSU_FW_VERSION_LEN	(8);

// ipmi_ret_t ipmiGetPSUCountTest(ipmi_netfn_t, ipmi_cmd_t,
//                                         ipmi_request_t,
//                                         ipmi_response_t,
//                                         ipmi_data_len_t,
//                                         ipmi_context_t);

// ipmi_ret_t ipmiGetPSUStatus(ipmi_netfn_t, ipmi_cmd_t,
//                                         ipmi_request_t,
//                                         ipmi_response_t,
//                                         ipmi_data_len_t,
//                                         ipmi_context_t);

void registerPSUFunctions();

#pragma pack(push, 1)
struct GetPSUNumReq
{
    uint8_t psuCmd;
};

struct GetPSUNumRes
{
    uint8_t completeCode;
    uint8_t psuNum;
};

struct GetPSUStatusReq
{
    uint8_t psuIndex;
};

struct InspurPSUInfo_T
{
	uint8_t  Index;			//PSU id
	uint8_t  Present;	
	uint8_t  WorkMode;		//workd mode:active/standby
	uint16_t StatusWord;
	uint16_t Temperature;
	uint16_t InputPower;		//Pin
	uint16_t OutputPower;		//Pout
	uint16_t InputVolt;		//Vin
	uint16_t OutputVolt;		//Vout
	uint16_t InputCurrent;	//Iin
	uint16_t OutputCurrent;	//Iout
	uint16_t OutputPowerMax;
	char   Manufacturer[PSU_MFR_LEN];		//MFR:Delta
	char   Model[PSU_MODEL_LEN];			//model:DSP-1600AB-7A
	char   SerialNum[PSU_SN_LEN];			//SN:HGQD1145000034
	char   FWVersion[PSU_FW_VERSION_LEN];	//firmware version:6.860
	char   PartNum[PSU_PN_LEN];
};

struct GetPSUStatusRes
{
    uint8_t completeCode;
    InspurPSUInfo_T PSUInfo; 
};
# pragma pack(pop)
}