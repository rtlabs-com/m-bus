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

set_target_properties (mbus_test
  PROPERTIES
  C_STANDARD 99
  CXX_STANDARD 11
  )

target_sources(mbus_test PRIVATE
  # Unit tests
  test_mbus.cpp
  test_slave.cpp

  # Slave fixture
  ${MBUS_SOURCE_DIR}/sample/slave.c

  # Test utils
  mocks.h
  mocks.cpp
  test_util.h

  # Testrunner
  mbus_test.cpp
  )

# Rebuild units to be tested with UNIT_TEST flag set. This is used to
# mock external dependencies.
target_sources(mbus_test PRIVATE
  ${MBUS_SOURCE_DIR}/src/mbus.c
  ${MBUS_SOURCE_DIR}/src/mb_slave.c
  )

get_target_property(MBUS_OPTIONS mbus COMPILE_OPTIONS)
target_compile_options(mbus_test PRIVATE
  -DUNIT_TEST
  -DMB_TCP_LOG=LOG_STATE_OFF
  -DMB_RTU_LOG=LOG_STATE_OFF
  ${MBUS_OPTIONS}
  )

target_include_directories(mbus_test
  PRIVATE
  ${MBUS_SOURCE_DIR}/src
  ${MBUS_BINARY_DIR}/src
  ${MBUS_SOURCE_DIR}/sample
  )

# Link against mbus target to get the cmake properties for that
# target, such as include paths and libraries.
target_link_libraries(mbus_test
  PRIVATE
  mbus
  )

# No need for gmock
set(BUILD_GMOCK OFF CACHE BOOL "")
