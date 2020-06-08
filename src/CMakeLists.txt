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

# NOTE: add headers to make them show up in an IDE
target_sources (mbus PRIVATE
  ${MBUS_SOURCE_DIR}/include/mb_master.h
  ${MBUS_SOURCE_DIR}/include/mb_slave.h
  ${MBUS_SOURCE_DIR}/include/mb_transport.h
  ${MBUS_SOURCE_DIR}/include/mb_rtu.h
  ${MBUS_SOURCE_DIR}/include/mb_tcp.h
  ${MBUS_SOURCE_DIR}/include/mb_error.h
  mbus.c
  mb_slave.c
  mb_transport.c
  mb_tcp.c
  mb_rtu.c
  mb_crc.c
  mb_crc.h
  mb_pdu.h
  )
