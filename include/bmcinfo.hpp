#pragma once
#include <cstdint>

#define BMC_VERSION_LENGTH 40
#define BMC_BUILDTIME_LENGTH 20

struct GetBMCVersionRes
{
	uint8_t version[BMC_VERSION_LENGTH];
    uint8_t buildtime[BMC_BUILDTIME_LENGTH];
};
