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

#include "mb_rtu.h"
#include <bsp.h>
#include <config.h>

static void mb_tx_enable (int level)
{
   /* This function controls the transceiver enabled state, if
      possible */
}

static void mb_tmr_init (uint32_t t1p5_us, uint32_t t3p5_us)
{
   /* This function enables the T1P5 and T3P5 timers (if required). */
}

static void mb_tmr_start (
   void (*t1p5_expired) (void * arg),
   void (*t3p5_expired) (void * arg),
   void * arg)
{
   /* This function starts the T1P5 and T3P5 timers, and arranges for
      the t1p5_expired and t3p5_expired functions to be called when
      the timers expire. This could also be managed by hardware
      directly if suitable hardware support exists. */
}

mb_transport_t * mb_rtu_create (void)
{
   mb_transport_t * rtu;
   static const mb_rtu_serial_cfg_t serial_cfg = {
      .baudrate = 115200,
      .parity = NONE,
   };
   static const mb_rtu_cfg_t mb_rtu_cfg = {
      .serial = "/sio1",
      .serial_cfg = &serial_cfg,
      .tx_enable = mb_tx_enable,
      .tmr_init = mb_tmr_init,
      .tmr_start = mb_tmr_start,
   };

   rtu = mb_rtu_init (&mb_rtu_cfg);
   return rtu;
}
