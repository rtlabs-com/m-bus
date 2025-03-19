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

if (EXISTS ports/rt-kernel/mb_${BSP}.c)
  set(BSP_SOURCE ports/rt-kernel/mb_${BSP}.c)
else()
  set(BSP_SOURCE ports/rt-kernel/mb_bsp.c)
endif()

target_sources(mb_master
  PRIVATE
  ${BSP_SOURCE}
  ports/rt-kernel/mb_cmds.c
  ports/rt-kernel/tcp_rtu_master.c
  )

target_sources(mb_rtu_slave
  PRIVATE
  ${BSP_SOURCE}
  ports/rt-kernel/rtu_slave.c
  )
