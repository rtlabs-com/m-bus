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

#include "mb_master.h"
#include "mb_pdu.h"

#include <drivers/dev.h>
#include <drivers/fd.h>

typedef struct mb_drv
{
   drv_t drv;
   mbus_t mbus;
} mb_drv_t;

static int _mb_open (drv_t * drv, const char * name, int flags, int mode)
{
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   return mb_transport_bringup (mb_drv->mbus.transport, name);
}

static int _mb_close (drv_t * drv, void * arg)
{
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   return mb_transport_shutdown (mb_drv->mbus.transport, (int)arg);
}

int mb_read (int fd, mb_address_t address, uint16_t quantity, void * buffer)
{
   drv_t * drv = fd_get_driver (fd);
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   int slave = (int)fd_get_arg (fd);
   int result;

   mtx_lock (drv->mtx);
   result = mbus_read (&mb_drv->mbus, slave, address, quantity, buffer);
   mtx_unlock (drv->mtx);

   return result;
}

int mb_write (int fd, mb_address_t address, uint16_t quantity, void * buffer)
{
   drv_t * drv = fd_get_driver (fd);
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   int slave = (int)fd_get_arg (fd);
   int result;

   mtx_lock (drv->mtx);
   result = mbus_write (&mb_drv->mbus, slave, address, quantity, buffer);
   mtx_unlock (drv->mtx);

   return result;
}

int mb_write_single (int fd, mb_address_t address, uint16_t value)
{
   drv_t * drv = fd_get_driver (fd);
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   int slave = (int)fd_get_arg (fd);
   int result;

   mtx_lock (drv->mtx);
   result = mbus_write_single (&mb_drv->mbus, slave, address, value);
   mtx_unlock (drv->mtx);

   return result;
}

int mb_loopback (int fd, uint16_t size, void * buffer)
{
   drv_t * drv = fd_get_driver (fd);
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   int slave = (int)fd_get_arg (fd);
   int result;

   mtx_lock (drv->mtx);
   result = mbus_loopback (&mb_drv->mbus, slave, size, buffer);
   mtx_unlock (drv->mtx);

   return result;
}

int mb_send_msg (int fd, const void * msg, uint8_t size)
{
   drv_t * drv = fd_get_driver (fd);
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   int slave = (int)fd_get_arg (fd);
   int result;

   mtx_lock (drv->mtx);
   result = mbus_send_msg (&mb_drv->mbus, slave, msg, size);
   mtx_unlock (drv->mtx);

   return result;
}

int mb_get_msg (int fd, void * msg, uint16_t size)
{
   drv_t * drv = fd_get_driver (fd);
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   int slave = (int)fd_get_arg (fd);
   int result;

   mtx_lock (drv->mtx);
   result = mbus_get_msg (&mb_drv->mbus, slave, msg, size);
   mtx_unlock (drv->mtx);

   return result;
}

void * mb_master_transport_get (int fd)
{
   drv_t * drv = fd_get_driver (fd);
   mb_drv_t * mb_drv = (mb_drv_t *)drv;
   return mb_drv->mbus.transport;
}

static const drv_ops_t mb_drv_ops = {
   .open = _mb_open,
   .read = NULL,
   .write = NULL,
   .close = _mb_close,
   .ioctl = NULL,
   .hotplug = NULL};

drv_t * mb_master_init (
   const char * name,
   const mb_master_cfg_t * cfg,
   mb_transport_t * transport)
{
   mb_drv_t * mb_drv;
   uint8_t * scratch;

   /* Allocate and initialise driver structure */
   mb_drv = malloc (sizeof (mb_drv_t));
   UASSERT (mb_drv != NULL, EMEM);

   /* Allocate scratch buffer */
   scratch = malloc (MAX_PDU_SIZE);
   UASSERT (scratch != NULL, EMEM);

   mbus_init (&mb_drv->mbus, cfg, transport, scratch);

   mb_drv->drv.ops = &mb_drv_ops;

   /* Install device driver */
   dev_install ((drv_t *)mb_drv, name);

   return (drv_t *)mb_drv;
}
