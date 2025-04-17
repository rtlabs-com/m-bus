#********************************************************************
#        _       _         _
#  _ __ | |_  _ | |  __ _ | |__   ___
# | '__|| __|(_)| | / _` || '_ \ / __|
# | |   | |_  _ | || (_| || |_) |\__ \
# |_|    \__|(_)|_| \__,_||_.__/ |___/
#
# www.rt-labs.com
# Copyright 2019 rt-labs AB, Sweden.
#
# This software is dual-licensed under GPLv3 and a commercial
# license. See the file LICENSE.md distributed with this software for
# full license information.
#*******************************************************************/

cmake_minimum_required (VERSION 3.28)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/tools")
project (MBUS VERSION 1.0.0)

# Default settings if this is the main project
if (CMAKE_PROJECT_NAME STREQUAL MBUS)
  include(CTest)

  # Make option visible in ccmake, cmake-gui
  option (BUILD_SHARED_LIBS "Build shared library" OFF)

  # Default to release build with debug info
  if (NOT CMAKE_BUILD_TYPE)
    set (CMAKE_BUILD_TYPE RelWithDebInfo CACHE STRING
      "Choose the type of build, options are: None Debug Release RelWithDebInfo MinSizeRel."
      FORCE)
  endif (NOT CMAKE_BUILD_TYPE)

  # Default to installing in build directory
  if (CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT)
    set(CMAKE_INSTALL_PREFIX ${MBUS_BINARY_DIR}/install
      CACHE PATH "Default install path" FORCE)
  endif()

  message(STATUS "Current build type is: ${CMAKE_BUILD_TYPE}")
  message(STATUS "Current install path is: ${CMAKE_INSTALL_PREFIX}")
  message(STATUS "Building for ${CMAKE_SYSTEM_NAME}")
endif()

include(AddOsal)
include(GenerateExportHeader)
include(CMakeDependentOption)

# Always use standard .o suffix
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

set(LOG_STATE_VALUES "ON;OFF")
set(LOG_LEVEL_VALUES "DEBUG;INFO;WARNING;ERROR")

set(LOG_LEVEL ERROR CACHE STRING "default log level")
set_property(CACHE LOG_LEVEL PROPERTY STRINGS ${LOG_LEVEL_VALUES})

set(MB_RTU_LOG ON CACHE STRING "rtu log")
set_property(CACHE MB_RTU_LOG PROPERTY STRINGS ${LOG_STATE_VALUES})

set(MB_TCP_LOG ON CACHE STRING "tcp log")
set_property(CACHE MB_TCP_LOG PROPERTY STRINGS ${LOG_STATE_VALUES})

# Generate version numbers
configure_file (
  version.h.in
  ${MBUS_BINARY_DIR}/src/version.h
  )

# Generate config options
configure_file (
  options.h.in
  ${MBUS_BINARY_DIR}/src/options.h
  )

# Add platform-dependent targets early, so they can be configured by
# platform
add_library(mbus "")

# Platform configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CMAKE_SYSTEM_NAME}.cmake)

generate_export_header(mbus
  BASE_NAME mb
  EXPORT_FILE_NAME ${MBUS_BINARY_DIR}/include/mb_export.h
  )

set_target_properties (mbus
  PROPERTIES
  C_STANDARD 99
  )

target_compile_features(mbus
  PUBLIC
  c_std_99
  PRIVATE
  cxx_std_11)

target_include_directories(mbus
  PUBLIC
  $<BUILD_INTERFACE:${MBUS_SOURCE_DIR}/include>
  $<BUILD_INTERFACE:${MBUS_BINARY_DIR}/include>
  $<INSTALL_INTERFACE:include>
  PRIVATE
  ${MBUS_BINARY_DIR}/src
  src
  )

target_link_libraries(mbus PUBLIC osal)

install (
  TARGETS mbus
  EXPORT MbusConfig
  DESTINATION lib
  )

install(
  EXPORT MbusConfig
  DESTINATION cmake
  )

install (FILES
  include/mb_master.h
  include/mb_slave.h
  include/mb_transport.h
  include/mb_rtu.h
  include/mb_tcp.h
  include/mb_error.h
  include/mbus.h
  ${MBUS_BINARY_DIR}/include/mb_export.h
  DESTINATION include
  )

add_subdirectory (src)
add_subdirectory (sample)

if (CMAKE_PROJECT_NAME STREQUAL MBUS AND BUILD_TESTING)
  add_subdirectory (test)
endif()

if (CMAKE_PROJECT_NAME STREQUAL MBUS)
  add_subdirectory(docs)
endif()

include (InstallRequiredSystemLibraries)
set (CPACK_RESOURCE_FILE_LICENSE "${MBUS_SOURCE_DIR}/LICENSE.md")
set (CPACK_PACKAGE_CONTACT info.modbus@rt-labs.com)
set (CPACK_PACKAGE_NAME mbus)
set (CPACK_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}-${CMAKE_SYSTEM_PROCESSOR})
include (CPack)
