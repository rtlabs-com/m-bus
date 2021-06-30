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
  src/ports/rt-kernel
  )

target_sources(mbus
  PRIVATE
  src/ports/rt-kernel/mbal_tcp.c
  src/ports/rt-kernel/mbal_rtu.c
  src/ports/rt-kernel/mb_master.c
  src/ports/rt-kernel/mb_cmds.c
  )

target_compile_options(mbus
  PRIVATE
  -Wall
  -Wextra
  -Werror
  -Wno-unused-parameter
  )

if (EXISTS ${MBUS_SOURCE_DIR}/src/ports/rt-kernel/mb_${BSP}.c)
  set(BSP_SOURCE src/ports/rt-kernel/mb_${BSP}.c)
else()
  set(BSP_SOURCE src/ports/rt-kernel/mb_bsp.c)
endif()

target_sources(mb_master
  PRIVATE
  ${BSP_SOURCE}
  src/ports/rt-kernel/tcp_rtu_master.c
  )

target_sources(mb_tcp_slave
  PRIVATE
  sample/slave.c
  sample/tcp_slave.c
  )

target_sources(mb_rtu_slave
  PRIVATE
  sample/slave.c
  src/ports/rt-kernel/mb_bsp.c
  src/ports/rt-kernel/rtu_slave.c
  )
