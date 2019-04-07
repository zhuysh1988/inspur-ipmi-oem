/*
// Copyright (c) 2018 Intel Corporation
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
*/

#pragma once

enum ipmi_net_fns
{
    netfunInspurAppOEM = 0x3C
};

enum IPMINetfnInspurOEMCmd
{
    // Get CPU EEPROM info .
    cmdGetEEPROMAllInfo = 0x00,
    // set CPU EEPROM info.
    cmdsetEEPROMInfo = 0x01,
    // get the number of PSUs.
    cmdGetPSUNumber = 0x2B,
    // get PSU status.
    cmdGetPSUStatus = 0x29,
    // get the number of SFPs.
    cmdGetSFPNumber = 0x40,
    // get the SFP present status.
    cmdGetSFPPrecentStatus = 0x41,
    // get the SFP low power mode.
    cmdGetSFPLowPowerMode = 0x42,
    // set SFP low power mode.
    cmdSetSFPLowPowerMode = 0x43,
    // reset SFP.
    cmdResetSFP = 0x44,
    // get SFP EEPROM info.
    cmdGetSFPEEPROMInfo = 0x45,
    // get SFP interrupt info.
    cmdGetSFPInterruptInfo = 0x46,
    // set System info.
    cmdSetSystemInfo = 0x50,
    // get System info. 
    cmdGetSystemInfo = 0x51,
    // get CPLD version info.
    cmdGetCPLDVersionInfo = 0x52,
    cmdGetBMCVersionInfo = 0x53,
    // get VR version info.
    cmdGetVRInfo = 0x54,
    // get fan info.
    cmdGetFan = 0x55,
    // set fan info.
    cmdSetFan = 0x56,
    // set mac address.
    cmdSetMACAddress = 0x60
};
