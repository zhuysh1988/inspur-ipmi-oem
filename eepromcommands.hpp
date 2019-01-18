#pragma once
#include <cstdint>
#include <main.hpp>

namespace inspur
{


/* Maximum length of a TLV value in bytes */
static constexpr const uint8_t TLV_VALUE_MAX_LEN = 255;

/**
 *  The TLV Types.
 *
 *  Keep these in sync with tlv_code_list in cmd_sys_eeprom.c
 */
static constexpr const uint8_t TLV_CODE_PRODUCT_NAME   = 0x21;
static constexpr const uint8_t TLV_CODE_PART_NUMBER    = 0x22;
static constexpr const uint8_t TLV_CODE_SERIAL_NUMBER  = 0x23;
static constexpr const uint8_t TLV_CODE_MAC_BASE       = 0x24;
static constexpr const uint8_t TLV_CODE_MANUF_DATE     = 0x25;
static constexpr const uint8_t TLV_CODE_DEVICE_VERSION = 0x26;
static constexpr const uint8_t TLV_CODE_LABEL_REVISION = 0x27;
static constexpr const uint8_t TLV_CODE_PLATFORM_NAME  = 0x28;
static constexpr const uint8_t TLV_CODE_ONIE_VERSION   = 0x29;
static constexpr const uint8_t TLV_CODE_MAC_SIZE       = 0x2A;
static constexpr const uint8_t TLV_CODE_MANUF_NAME     = 0x2B;
static constexpr const uint8_t TLV_CODE_MANUF_COUNTRY  = 0x2C;
static constexpr const uint8_t TLV_CODE_VENDOR_NAME    = 0x2D;
static constexpr const uint8_t TLV_CODE_DIAG_VERSION   = 0x2E;
static constexpr const uint8_t TLV_CODE_SERVICE_TAG    = 0x2F;
static constexpr const uint8_t TLV_CODE_VENDOR_EXT     = 0xFD;
static constexpr const uint8_t TLV_CODE_CRC_32         = 0xFE;

/*
 *  Struct for displaying the TLV codes and names.
 */
struct tlv_code_desc {
    uint8_t m_code;
    char* m_name;
};

/*
 *  List of TLV codes and names.
 */
static const struct tlv_code_desc tlv_code_list[] = {
    { TLV_CODE_PRODUCT_NAME	 , "Product Name"},
    { TLV_CODE_PART_NUMBER	 , "Part Number"},
    { TLV_CODE_SERIAL_NUMBER     , "Serial Number"},
    { TLV_CODE_MAC_BASE	         , "Base MAC Address"},
    { TLV_CODE_MANUF_DATE	 , "Manufacture Date"},
    { TLV_CODE_DEVICE_VERSION    , "Device Version"},
    { TLV_CODE_LABEL_REVISION    , "Label Revision"},
    { TLV_CODE_PLATFORM_NAME     , "Platform Name"},
    { TLV_CODE_ONIE_VERSION	 , "ONIE Version"},
    { TLV_CODE_MAC_SIZE	         , "MAC Addresses"},
    { TLV_CODE_MANUF_NAME	 , "Manufacturer"},
    { TLV_CODE_MANUF_COUNTRY     , "Country Code"},
    { TLV_CODE_VENDOR_NAME	 , "Vendor Name"},
    { TLV_CODE_DIAG_VERSION	 , "Diag Version"},
    { TLV_CODE_SERVICE_TAG       , "Service Tag"},
    { TLV_CODE_VENDOR_EXT	 , "Vendor Extension"},
    { TLV_CODE_CRC_32	         , "CRC-32"},
};





#pragma pack(push, 1)
struct GetEEPROMInfoReq
{
    uint8_t key;
    uint8_t value[maxArrayLength];
};

struct GetEEPROMInfoRes
{
    uint8_t completeCode;
    uint8_t eepContent[maxArrayLength];
};
# pragma pack(pop)
} // namespace inspur