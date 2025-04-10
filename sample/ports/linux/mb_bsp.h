/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2020 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef MB_BSP_H
#define MB_BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_rtu.h"

mb_transport_t * mb_rtu_create (
   const char * device,
   mb_rtu_serial_cfg_t * serial_cfg);

#ifdef __cplusplus
}
#endif

#endif /* MB_BSP_H */
