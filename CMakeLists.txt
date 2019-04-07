cmake_minimum_required (VERSION 3.5 FATAL_ERROR)

cmake_policy (SET CMP0054 NEW)

option (YOCTO "Use YOCTO depedencies system" OFF)
include (ExternalProject)
set (CMAKE_CXX_STANDARD 17)
set (CMAKE_CXX_STANDARD_REQUIRED ON)
set (CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake ${CMAKE_MODULE_PATH})

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fno-rtti")

project (inspur-ipmi-oem CXX)

add_definitions (-DBOOST_ERROR_CODE_HEADER_ONLY)
add_definitions (-DBOOST_SYSTEM_NO_DEPRECATED)
add_definitions (-DBOOST_ALL_NO_LIB)
add_definitions (-DBOOST_NO_RTTI)
add_definitions (-DBOOST_NO_TYPEID)
add_definitions (-DBOOST_ASIO_DISABLE_THREADS)
add_definitions (-DBOOST_COROUTINES_NO_DEPRECATION_WARNING)

if (NOT YOCTO)
    configure_file (CMakeLists.txt.in 3rdparty/CMakeLists.txt)
    execute_process (
        COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rdparty
    )
    execute_process (
        COMMAND ${CMAKE_COMMAND} --build .
        WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/3rdparty
    )

    set (CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/prefix ${CMAKE_PREFIX_PATH})
    include_directories (${CMAKE_BINARY_DIR}/prefix/include)
    link_directories (${CMAKE_BINARY_DIR}/prefix/lib)

    include_directories (${CMAKE_BINARY_DIR}/sdbusplus-src)
    link_directories (${CMAKE_BINARY_DIR}/sdbusplus-src/.libs)
    include_directories (${CMAKE_BINARY_DIR}/phosphor-logging-src)
    link_directories (${CMAKE_BINARY_DIR}/phosphor-logging-src/.libs)
    include_directories (${CMAKE_BINARY_DIR}/phosphor-ipmi-host/include)
    include_directories (${CMAKE_BINARY_DIR}/ipmid/user_channel)
    include_directories (${CMAKE_BINARY_DIR}) # link_directories
                                              # (${CMAKE_BINARY_DIR}/sdbusplus-
                                              # src/.libs)
endif ()

if (YOCTO)
    find_package (PkgConfig REQUIRED)
    pkg_check_modules (LOGGING phosphor-logging REQUIRED)
    include_directories (${LOGGING_INCLUDE_DIRS})
    link_directories (${LOGGING_LIBRARY_DIRS})

    pkg_check_modules (LIBIPMID libipmid REQUIRED)
    include_directories (${LIBIPMID_INCLUDE_DIRS})
    link_directories (${LIBIPMID_LIBRARY_DIRS})

endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

if (NOT YOCTO)
    set (SENSOR_TEST_SRC tests/test_sensorcommands.cpp)

    find_package (GTest REQUIRED)

    enable_testing ()

    add_executable (runSensorTests ${SENSOR_TEST_SRC})
    add_test (NAME test_sensorcommands COMMAND runSensorTests)
    target_link_libraries (
        runSensorTests ${GTEST_BOTH_LIBRARIES} ${CMAKE_THREAD_LIBS_INIT}
        phosphor_logging sdbusplus -lsystemd
    )
endif ()

include_directories (${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library (
    zinspuroemcmds SHARED src/psucommands.cpp src/bios.cpp src/bmcinfo.cpp 
    src/i2c.cpp src/inspur_cmds.cpp src/sfpcommands.cpp
)
set_target_properties (zinspuroemcmds PROPERTIES VERSION "0.1.0")
set_target_properties (zinspuroemcmds PROPERTIES SOVERSION "0")
target_link_libraries (zinspuroemcmds ipmid)
target_link_libraries (zinspuroemcmds sdbusplus)
target_link_libraries (zinspuroemcmds phosphor_logging)
target_link_libraries (zinspuroemcmds -luserlayer)
target_link_libraries (zinspuroemcmds -lchannellayer)

install (TARGETS zinspuroemcmds DESTINATION lib/ipmid-providers)
