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

# FIXME: OSAL requires Threads but transitive dependency is not
# exported by cmake
find_package(Threads)

# Options
option (USE_TRACE
  "Add tracepoints"
  OFF)

if (USE_TRACE)
  find_package(LTTngUST)
  add_compile_definitions(USE_TRACE)
endif()

target_include_directories(mbus
  PRIVATE
  src/ports/linux
  )

target_sources(mbus
  PRIVATE
  src/ports/linux/mbal_tcp.c
  src/ports/linux/mbal_rtu.c
  $<$<BOOL:${USE_TRACE}>:src/ports/linux/mb-tp.c>
  )

target_compile_options(mbus
  PRIVATE
  -Wall
  -Wextra
  -Werror
  -Wno-unused-parameter
  INTERFACE
  $<$<CONFIG:Coverage>:--coverage -g>
  )

target_link_libraries(mbus
  PUBLIC
  $<$<BOOL:${USE_TRACE}>:LTTng::UST>
  INTERFACE
  $<$<CONFIG:Coverage>:--coverage -g>
  )

if (BUILD_TESTING)
  set(GOOGLE_TEST_INDIVIDUAL TRUE)
endif()
