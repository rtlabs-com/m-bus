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

target_compile_options(mb_sample
  INTERFACE
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

target_sources(mb_master
  PRIVATE
  ports/windows/mb_bsp.c
  ports/windows/tcp_rtu_master.c
  )

target_sources(mb_rtu_slave
  PRIVATE
  ports/windows/mb_bsp.c
  ports/windows/rtu_slave.c
  )
