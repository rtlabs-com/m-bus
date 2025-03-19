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

cmake_minimum_required (VERSION 3.14)
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/tools")
project (MBUS_SAMPLES)


if (NOT TARGET osal)
  find_package(Osal REQUIRED)
endif()

if (NOT TARGET mbus)
  find_package(Mbus REQUIRED)
endif()

# Always use standard .o suffix
set(CMAKE_C_OUTPUT_EXTENSION_REPLACE 1)
set(CMAKE_CXX_OUTPUT_EXTENSION_REPLACE 1)

# Add platform-dependent targets early, so they can be configured by
# platform
add_library(mb_sample INTERFACE)
add_executable(mb_master)
add_executable(mb_tcp_slave)
add_executable(mb_rtu_slave)

# Platform configuration
include(${CMAKE_CURRENT_SOURCE_DIR}/cmake/${CMAKE_SYSTEM_NAME}.cmake)

target_include_directories(mb_sample
  INTERFACE
  .
  )

target_link_libraries(mb_sample
  INTERFACE
  mbus
  )

target_compile_features(mb_sample
  INTERFACE
  c_std_99)

target_sources(mb_tcp_slave
  PRIVATE
  slave.c
  tcp_slave.c
  )

target_sources(mb_rtu_slave
  PRIVATE
  slave.c
  )

target_link_libraries(mb_master
  PRIVATE
  mb_sample
  )

target_link_libraries(mb_tcp_slave
  PRIVATE
  mb_sample
  )

target_link_libraries(mb_rtu_slave
  PRIVATE
  mb_sample
  )

install (
  TARGETS
  mb_master
  mb_tcp_slave
  mb_rtu_slave
  DESTINATION bin)
