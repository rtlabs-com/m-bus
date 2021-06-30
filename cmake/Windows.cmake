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
  /WX
  /wd4200
  /D _CRT_SECURE_NO_WARNINGS
  )

target_link_libraries(mbus
  PUBLIC
  wsock32
  ws2_32)

target_sources(mb_master
  PRIVATE
  src/ports/windows/mb_bsp.c
  src/ports/windows/tcp_rtu_master.c
  )

target_sources(mb_tcp_slave
  PRIVATE
  sample/slave.c
  sample/tcp_slave.c
  )

target_sources(mb_rtu_slave
  PRIVATE
  sample/slave.c
  src/ports/windows/mb_bsp.c
  src/ports/windows/rtu_slave.c
  )

if (BUILD_TESTING)
  set(GOOGLE_TEST_INDIVIDUAL TRUE)
endif()
