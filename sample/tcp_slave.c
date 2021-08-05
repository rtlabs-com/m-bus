/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2015 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "slave.h"
#include "mb_tcp.h"
#include "osal.h"

mb_slave_t * mb_tcp_start (void)
{
   mb_slave_t * slave;
   mb_transport_t * tcp;
   static const mb_tcp_cfg_t mb_tcp_cfg = {
      .port = 8502,
   };

   tcp   = mb_tcp_init (&mb_tcp_cfg);
   slave = mb_slave_init (&mb_slave_cfg, tcp);
   return slave;
}

int main (void)
{
   mb_slave_t * s;

   s = mb_tcp_start();
   os_usleep (20 * 1000 * 1000);
   mb_slave_shutdown (s);
}
