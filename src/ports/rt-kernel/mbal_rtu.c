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
#include "options.h"

#include "osal.h"
#include "osal_log.h"

#include <errno.h>
#include <fcntl.h>
#include <drivers/ioctl.h>
#include <drivers/fd.h>
#include <drivers/sio/sio.h>
#include <unistd.h>

ssize_t os_rtu_write (int fd, const void * buffer, size_t size)
{
   ssize_t nwrite;
   nwrite = write (fd, buffer, size);
   return nwrite;
}

ssize_t os_rtu_read (int fd, void * buffer, size_t size)
{
   ssize_t nread;
   nread = read (fd, buffer, size);
   return nread;
}

void os_rtu_tx_drain (int fd, size_t size)
{
   int is_empty = 0;

   /* os_event_wait (rtu->flags, FLAG_TX_EMPTY, &flags, OS_WAIT_FOREVER); */

   while (!is_empty)
      ioctl (fd, IOCTL_SIO_IS_TX_EMPTY, &is_empty);
}

ssize_t os_rtu_rx_avail (int fd)
{
   ssize_t navail = 0;
   if (ioctl (fd, IOCTL_SIO_NREAD, &navail) < 0)
   {
      return -1;
   }
   return navail;
}

void os_rtu_set_serial_cfg (int fd, const mb_rtu_serial_cfg_t * cfg)
{
   drv_t * drv = fd_get_driver (fd);
   sio_cfg_t sio_cfg;
   sio_parity_t parity;

   switch (cfg->parity)
   {
   case ODD:
      parity = Odd;
      break;
   case EVEN:
      parity = Even;
      break;
   case NONE:
      parity = None;
      break;
   default:
      parity = ODD;
      break;
   }

   sio_cfg.baudrate = cfg->baudrate;
   sio_cfg.databits = 8;
   sio_cfg.parity = parity;
   sio_cfg.stopbits = 1;

   sio_set_cfg (drv, &sio_cfg);
}

int os_rtu_open (const char * name, void * arg)
{
   int fd;
   ioctl_hook_t hook;

   fd = open (name, O_RDWR, 0);
   UASSERT (fd != -1, EARG);

   /* Install serial hooks */

   hook.func = mb_tx_hook;
   hook.arg = arg;
   ioctl (fd, IOCTL_SIO_TX_HOOK, &hook);

   hook.func = mb_rx_hook;
   hook.arg = arg;
   ioctl (fd, IOCTL_SIO_RX_HOOK, &hook);

   return fd;
}
