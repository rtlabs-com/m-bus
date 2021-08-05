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

#ifndef MBAL_RTU_H
#define MBAL_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mbal_sys.h"
#include "mb_rtu.h"

int mb_rx_hook (void * arg, void * data);
int mb_tx_hook (void * arg, void * data);

ssize_t os_rtu_write (int fd, const void * buffer, size_t size);

ssize_t os_rtu_read (int fd, void * buffer, size_t size);

void os_rtu_tx_drain (int fd, size_t size);

ssize_t os_rtu_rx_avail (int fd);

void os_rtu_set_serial_cfg (int fd, const mb_rtu_serial_cfg_t * cfg);

int os_rtu_open (const char * name, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* MBAL_RTU_H */
