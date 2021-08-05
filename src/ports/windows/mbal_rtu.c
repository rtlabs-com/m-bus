/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2012 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "mbal_rtu.h"

ssize_t os_rtu_write (int fd, const void * buffer, size_t size)
{
   return -1;
}

ssize_t os_rtu_read (int fd, void * buffer, size_t size)
{
   return -1;
}

void os_rtu_tx_drain (int fd, size_t size)
{
}

ssize_t os_rtu_rx_avail (int fd)
{
   return -1;
}

void os_rtu_set_serial_cfg (int fd, const mb_rtu_serial_cfg_t * cfg)
{
}

int os_rtu_open (const char * name, void * arg)
{
   return -1;
}
