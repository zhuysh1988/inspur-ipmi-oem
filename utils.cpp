#include "utils.hpp"

#include <arpa/inet.h>
#include <dirent.h>
#include <net/if.h>

#include <algorithm>
#include <phosphor-logging/elog-errors.hpp>
#include <phosphor-logging/log.hpp>
#include <sdbusplus/message/types.hpp>
#include <xyz/openbmc_project/Common/error.hpp>

namespace ipmi
{

using namespace phosphor::logging;
using namespace sdbusplus::xyz::openbmc_project::Common::Error;
namespace variant_ns = sdbusplus::message::variant_ns;

// TODO There may be cases where an interface is implemented by multiple
//  objects,to handle such cases we are interested on that object
//  which are on interested busname.
//  Currently mapper doesn't give the readable busname(gives busid) so we can't
//  use busname to find the object,will do later once the support is there.

DbusObjectInfo getDbusObject(sdbusplus::bus::bus& bus,
                             const std::string& interface,
                             const std::string& serviceRoot,
                             const std::string& match)
{
    std::vector<DbusInterface> interfaces;
    interfaces.emplace_back(interface);

    auto depth = 0;

    auto mapperCall = bus.new_method_call(MAPPER_BUS_NAME, MAPPER_OBJ,
                                          MAPPER_INTF, "GetSubTree");

    mapperCall.append(serviceRoot, depth, interfaces);

    auto mapperReply = bus.call(mapperCall);
    if (mapperReply.is_method_error())
    {
        log<level::ERR>("Error in mapper call");
        elog<InternalFailure>();
    }

    ObjectTree objectTree;
    mapperReply.read(objectTree);

    if (objectTree.empty())
    {
        log<level::ERR>("No Object has implemented the interface",
                        entry("INTERFACE=%s", interface.c_str()));
        elog<InternalFailure>();
    }

    DbusObjectInfo objectInfo;

    // if match is empty then return the first object
    if (match == "")
    {
        objectInfo = std::make_pair(
            objectTree.begin()->first,
            std::move(objectTree.begin()->second.begin()->first));
        return objectInfo;
    }

    // else search the match string in the object path
    auto found = std::find_if(
        objectTree.begin(), objectTree.end(), [&match](const auto& object) {
            return (object.first.find(match) != std::string::npos);
        });

    if (found == objectTree.end())
    {
        log<level::ERR>("Failed to find object which matches",
                        entry("MATCH=%s", match.c_str()));
        elog<InternalFailure>();
        // elog<> throws an exception.
    }

    return make_pair(found->first, std::move(found->second.begin()->first));
}

Value getDbusProperty(sdbusplus::bus::bus& bus, const std::string& service,
                      const std::string& objPath, const std::string& interface,
                      const std::string& property)
{

    Value value;

    auto method = bus.new_method_call(service.c_str(), objPath.c_str(),
                                      PROP_INTF, METHOD_GET);

    method.append(interface, property);

    auto reply = bus.call(method);

    if (reply.is_method_error())
    {
        log<level::ERR>("Failed to get property",
                        entry("PROPERTY=%s", property.c_str()),
                        entry("PATH=%s", objPath.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        elog<InternalFailure>();
    }

    reply.read(value);

    return value;
}

PropertyMap getAllDbusProperties(sdbusplus::bus::bus& bus,
                                 const std::string& service,
                                 const std::string& objPath,
                                 const std::string& interface)
{
    PropertyMap properties;

    auto method = bus.new_method_call(service.c_str(), objPath.c_str(),
                                      PROP_INTF, METHOD_GET_ALL);

    method.append(interface);

    auto reply = bus.call(method);

    if (reply.is_method_error())
    {
        log<level::ERR>("Failed to get all properties",
                        entry("PATH=%s", objPath.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        elog<InternalFailure>();
    }

    reply.read(properties);
    return properties;
}

ObjectValueTree getManagedObjects(sdbusplus::bus::bus& bus,
                                  const std::string& service,
                                  const std::string& objPath)
{
    ipmi::ObjectValueTree interfaces;

    auto method = bus.new_method_call(service.c_str(), objPath.c_str(),
                                      "org.freedesktop.DBus.ObjectManager",
                                      "GetManagedObjects");

    auto reply = bus.call(method);

    if (reply.is_method_error())
    {
        log<level::ERR>("Failed to get managed objects",
                        entry("PATH=%s", objPath.c_str()));
        elog<InternalFailure>();
    }

    reply.read(interfaces);
    return interfaces;
}

void setDbusProperty(sdbusplus::bus::bus& bus, const std::string& service,
                     const std::string& objPath, const std::string& interface,
                     const std::string& property, const Value& value)
{
    auto method = bus.new_method_call(service.c_str(), objPath.c_str(),
                                      PROP_INTF, METHOD_SET);

    method.append(interface, property, value);

    if (!bus.call(method))
    {
        log<level::ERR>("Failed to set property",
                        entry("PROPERTY=%s", property.c_str()),
                        entry("PATH=%s", objPath.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        elog<InternalFailure>();
    }
}

ServiceCache::ServiceCache(const std::string& intf, const std::string& path) :
    intf(intf), path(path), cachedService(std::experimental::nullopt),
    cachedBusName(std::experimental::nullopt)
{
}

ServiceCache::ServiceCache(std::string&& intf, std::string&& path) :
    intf(std::move(intf)), path(std::move(path)),
    cachedService(std::experimental::nullopt),
    cachedBusName(std::experimental::nullopt)
{
}

const std::string& ServiceCache::getService(sdbusplus::bus::bus& bus)
{
    if (!isValid(bus))
    {
        cachedBusName = bus.get_unique_name();
        cachedService = ::ipmi::getService(bus, intf, path);
    }
    return cachedService.value();
}

void ServiceCache::invalidate()
{
    cachedBusName = std::experimental::nullopt;
    cachedService = std::experimental::nullopt;
}

sdbusplus::message::message
    ServiceCache::newMethodCall(sdbusplus::bus::bus& bus, const char* intf,
                                const char* method)
{
    return bus.new_method_call(getService(bus).c_str(), path.c_str(), intf,
                               method);
}

bool ServiceCache::isValid(sdbusplus::bus::bus& bus) const
{
    return cachedService && cachedBusName == bus.get_unique_name();
}

std::string getService(sdbusplus::bus::bus& bus, const std::string& intf,
                       const std::string& path)
{
    auto mapperCall =
        bus.new_method_call("xyz.openbmc_project.ObjectMapper",
                            "/xyz/openbmc_project/object_mapper",
                            "xyz.openbmc_project.ObjectMapper", "GetObject");

    mapperCall.append(path);
    mapperCall.append(std::vector<std::string>({intf}));

    auto mapperResponseMsg = bus.call(mapperCall);

    if (mapperResponseMsg.is_method_error())
    {
        throw std::runtime_error("ERROR in mapper call");
    }

    std::map<std::string, std::vector<std::string>> mapperResponse;
    mapperResponseMsg.read(mapperResponse);

    if (mapperResponse.begin() == mapperResponse.end())
    {
        throw std::runtime_error("ERROR in reading the mapper response");
    }

    return mapperResponse.begin()->first;
}

ipmi::ObjectTree getAllDbusObjects(sdbusplus::bus::bus& bus,
                                   const std::string& serviceRoot,
                                   const std::string& interface,
                                   const std::string& match)
{
    std::vector<std::string> interfaces;
    interfaces.emplace_back(interface);

    auto depth = 0;

    auto mapperCall = bus.new_method_call(MAPPER_BUS_NAME, MAPPER_OBJ,
                                          MAPPER_INTF, "GetSubTree");

    mapperCall.append(serviceRoot, depth, interfaces);

    auto mapperReply = bus.call(mapperCall);
    if (mapperReply.is_method_error())
    {
        log<level::ERR>("Error in mapper call",
                        entry("SERVICEROOT=%s", serviceRoot.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));

        elog<InternalFailure>();
    }

    ObjectTree objectTree;
    mapperReply.read(objectTree);

    for (auto it = objectTree.begin(); it != objectTree.end();)
    {
        if (it->first.find(match) == std::string::npos)
        {
            it = objectTree.erase(it);
        }
        else
        {
            ++it;
        }
    }

    return objectTree;
}

void deleteAllDbusObjects(sdbusplus::bus::bus& bus,
                          const std::string& serviceRoot,
                          const std::string& interface,
                          const std::string& match)
{
    try
    {
        auto objectTree = getAllDbusObjects(bus, serviceRoot, interface, match);

        for (auto& object : objectTree)
        {
            method_no_args::callDbusMethod(bus, object.second.begin()->first,
                                           object.first, DELETE_INTERFACE,
                                           "Delete");
        }
    }
    catch (InternalFailure& e)
    {
        log<level::INFO>("Unable to delete the objects having",
                         entry("INTERFACE=%s", interface.c_str()),
                         entry("SERVICE=%s", serviceRoot.c_str()));
    }
}

ObjectTree getAllAncestors(sdbusplus::bus::bus& bus, const std::string& path,
                           InterfaceList&& interfaces)
{
    auto convertToString = [](InterfaceList& interfaces) -> std::string {
        std::string intfStr;
        for (const auto& intf : interfaces)
        {
            intfStr += "," + intf;
        }
        return intfStr;
    };

    auto mapperCall = bus.new_method_call(MAPPER_BUS_NAME, MAPPER_OBJ,
                                          MAPPER_INTF, "GetAncestors");
    mapperCall.append(path, interfaces);

    auto mapperReply = bus.call(mapperCall);
    if (mapperReply.is_method_error())
    {
        log<level::ERR>(
            "Error in mapper call", entry("PATH=%s", path.c_str()),
            entry("INTERFACES=%s", convertToString(interfaces).c_str()));

        elog<InternalFailure>();
    }

    ObjectTree objectTree;
    mapperReply.read(objectTree);

    if (objectTree.empty())
    {
        log<level::ERR>(
            "No Object has implemented the interface",
            entry("PATH=%s", path.c_str()),
            entry("INTERFACES=%s", convertToString(interfaces).c_str()));
        elog<InternalFailure>();
    }

    return objectTree;
}

namespace method_no_args
{

void callDbusMethod(sdbusplus::bus::bus& bus, const std::string& service,
                    const std::string& objPath, const std::string& interface,
                    const std::string& method)

{
    auto busMethod = bus.new_method_call(service.c_str(), objPath.c_str(),
                                         interface.c_str(), method.c_str());

    auto reply = bus.call(busMethod);

    if (reply.is_method_error())
    {
        log<level::ERR>("Failed to execute method",
                        entry("METHOD=%s", method.c_str()),
                        entry("PATH=%s", objPath.c_str()),
                        entry("INTERFACE=%s", interface.c_str()));
        elog<InternalFailure>();
    }
}

} // namespace method_no_args
} // namespace ipmi
