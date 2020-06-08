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
#include "osal.h"
#include <bsp.h>
#include <config.h>

#define TIMER_T1P5 2 /* STM32 timer to use as t1p5 */
#define TIMER_T3P5 5 /* STM32 timer to use as t3p5 */

static uint32_t t1p5_match = 0;
static uint32_t t3p5_match = 0;

static void (*t1p5_callback) (void * arg);
static void (*t3p5_callback) (void * arg);

static void t1p5_fn (void * arg)
{
   timer_stop (TIMER_T1P5);
   t1p5_callback (arg);
}

static void t3p5_fn (void * arg)
{
   timer_stop (TIMER_T3P5);
   t3p5_callback (arg);
}

static void mb_tx_enable (int level)
{
   /* This function controls the transceiver enabled state, if
      possible */
#if 0
   gpio_set (GPIO_TX_EN, level);
#endif
}

static uint32_t mb_tmr_compute (uint32_t us)
{
   uint32_t f = 2 * CFG_PCLK1_FREQUENCY;  /* Timer frequency */
   uint32_t m = us * (f / (1000 * 1000)); /* Match value */
   return m;
}

static void mb_tmr_init (uint32_t t1p5_us, uint32_t t3p5_us)
{
   t1p5_match = mb_tmr_compute (t1p5_us);
   t3p5_match = mb_tmr_compute (t3p5_us);
}

static void mb_tmr_start (
   void (*t1p5_expired) (void * arg),
   void (*t3p5_expired) (void * arg),
   void * arg)
{
   timer_stop (TIMER_T1P5);
   timer_stop (TIMER_T3P5);

   if (t1p5_expired)
   {
      t1p5_callback = t1p5_expired;
      timer_init (TIMER_T1P5, t1p5_fn, arg);
      timer_start (TIMER_T1P5, 0, t1p5_match);
   }

   if (t3p5_expired)
   {
      t3p5_callback = t3p5_expired;
      timer_init (TIMER_T3P5, t3p5_fn, arg);
      timer_start (TIMER_T3P5, 0, t3p5_match);
   }
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

   RCC_APB1ENR |= APB1_TIM5 | APB1_TIM4;

   rtu = mb_rtu_init (&mb_rtu_cfg);
   return rtu;
}
