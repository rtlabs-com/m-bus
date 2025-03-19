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

target_include_directories(mbus
  PRIVATE
  src/ports/windows
  )

target_sources(mbus
  PRIVATE
  src/ports/windows/mbal_tcp.c
  src/ports/windows/mbal_rtu.c
  )

target_compile_options(mbus
  PRIVATE
  $<$<C_COMPILER_ID:MSVC>:
    /WX
    /wd4200
    /D _CRT_SECURE_NO_WARNINGS
  >

  $<$<C_COMPILER_ID:GNU>:
    -Wall
    -Wextra
    -Werror
    -Wno-unused-parameter
  >
  )

target_link_libraries(mbus
  PUBLIC
  wsock32
  ws2_32)

if (BUILD_TESTING)
  set(GOOGLE_TEST_INDIVIDUAL TRUE)
endif()
