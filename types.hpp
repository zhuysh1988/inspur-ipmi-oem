#pragma once

#include <stdint.h>

#include <map>
#include <sdbusplus/server.hpp>
#include <string>

namespace ipmi
{

using DbusObjectPath = std::string;
using DbusService = std::string;
using DbusInterface = std::string;
using DbusObjectInfo = std::pair<DbusObjectPath, DbusService>;
using DbusProperty = std::string;

using Value = sdbusplus::message::variant<bool, uint8_t, int16_t, uint16_t,
                                          int32_t, uint32_t, int64_t, uint64_t,
                                          double, std::string>;

using PropertyMap = std::map<DbusProperty, Value>;

using ObjectTree =
    std::map<DbusObjectPath, std::map<DbusService, std::vector<DbusInterface>>>;

using InterfaceList = std::vector<std::string>;

using DbusInterfaceMap = std::map<DbusInterface, PropertyMap>;

using ObjectValueTree =
    std::map<sdbusplus::message::object_path, DbusInterfaceMap>;
} // namespace ipmi
