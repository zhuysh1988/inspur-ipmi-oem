#include <ipmid/api.h>
#include "bmcinfo.hpp"
#include <string>
#include<iostream>
#include<fstream>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include "ipmid/utils.hpp"
using namespace phosphor::logging;
using namespace std;
void register_netfn_bmcinfo_functions() __attribute__((constructor));


void getBmcVersionInfo(void *response)
{
    GetBMCVersionRes version_info;
    memset(&version_info,0x00,sizeof(GetBMCVersionRes));
    char filename[] = "/usr/lib/os-release";
    char buffer[256];
    string buildtime,version; 
	ifstream in(filename);  
	if (! in.is_open())  
	{ 
        cout << "Error opening version file"; 
    }  
	while (!in.eof() )  
	{  
		in.getline(buffer,256);  
		string line(buffer);

        //find buildtime
        auto i  = line.find("BUILDTIME=",0);
		if (i!= string::npos )
		{
			int time_start = line.find('"')+1;
			int time_end = line.size()-time_start-1;
            buildtime = line.substr(time_start,time_end);
		}

        //find version
        i = line.find("VERSION=",0);
        if (i!= string::npos )
		{
			int version_start = line.find('"')+1;
			int version_end = line.size()-version_start-1;
            version = line.substr(version_start,version_end);
		}
	}  
	in.close();
    memcpy(version_info.version,version.data(),version.size());
    memcpy(version_info.buildtime,buildtime.data(),buildtime.size());
    memcpy(response,&version_info,sizeof(GetBMCVersionRes));

}


ipmi_ret_t ipmi_get_bmc_version_info(ipmi_netfn_t netfn, ipmi_cmd_t cmd,
                                    ipmi_request_t request, ipmi_response_t response,
                                    ipmi_data_len_t data_len, ipmi_context_t context)
{
    if(*data_len != 0 )
    {
        *data_len = 0;
        return IPMI_CC_REQ_DATA_LEN_INVALID;
    }
    try
    {
        getBmcVersionInfo(response);
    }
    catch (std::exception& e)
    {
        log<level::ERR>(e.what());
        return IPMI_CC_UNSPECIFIED_ERROR;
    }

    // Status code.
    ipmi_ret_t rc = IPMI_CC_OK;
    *data_len = sizeof(GetBMCVersionRes);
    return rc;
}



void register_netfn_bmcinfo_functions()
{
    // get bmc version info
    ipmiPrintAndRegister(netfunInspurAppOEM, cmdGetBMCVersionInfo, NULL,
                           ipmi_get_bmc_version_info, PRIVILEGE_ADMIN);


}
