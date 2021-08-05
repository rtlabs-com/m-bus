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

#include "mb_bsp.h"
#include "osal.h"

static os_timer_t * tmr1p5;
static void (*t1p5_callback) (void * arg);

static os_timer_t * tmr3p5;
static void (*t3p5_callback) (void * arg);

static void * tmr_arg;

static void tmr1p5_expired (os_timer_t * tmr, void * arg)
{
   if (t1p5_callback)
      t1p5_callback (tmr_arg);
}

static void tmr3p5_expired (os_timer_t * tmr, void * arg)
{
   if (t3p5_callback)
      t3p5_callback (tmr_arg);
}

static void mb_tx_enable (int level)
{
   /* This function controls the transceiver enabled state, if
      possible */
}

static void mb_tmr_init (uint32_t t1p5, uint32_t t3p5)
{
   os_timer_set (tmr1p5, t1p5);
   os_timer_set (tmr3p5, t3p5);
}

static void mb_tmr_start (
   void (*t1p5_expired) (void * arg),
   void (*t3p5_expired) (void * arg),
   void * arg)
{
   tmr_arg = arg;

   if (t1p5_expired)
   {
      t1p5_callback = t1p5_expired;
      os_timer_start (tmr1p5);
   }

   if (t3p5_expired)
   {
      t3p5_callback = t3p5_expired;
      os_timer_start (tmr3p5);
   }
}

mb_transport_t * mb_rtu_create (
   const char * device,
   mb_rtu_serial_cfg_t * serial_cfg)
{
   mb_transport_t * rtu;
   mb_rtu_cfg_t rtu_cfg;

   rtu_cfg.serial = device;
   rtu_cfg.serial_cfg = serial_cfg;
   rtu_cfg.tx_enable = mb_tx_enable;
   rtu_cfg.tmr_init = mb_tmr_init;
   rtu_cfg.tmr_start = mb_tmr_start;

   tmr1p5 = os_timer_create (0, tmr1p5_expired, NULL, true);
   tmr3p5 = os_timer_create (0, tmr3p5_expired, NULL, true);

   rtu = mb_rtu_init (&rtu_cfg);
   return rtu;
}
