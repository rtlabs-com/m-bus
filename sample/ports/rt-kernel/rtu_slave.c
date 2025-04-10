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
#include "mb_bsp.h"
#include "osal.h"

int main (void)
{
   mb_slave_t * slave;
   mb_transport_t * rtu;

   rtu = mb_rtu_create();
   slave = mb_slave_init (&mb_slave_cfg, rtu);
   (void)slave;

#if 0
   os_usleep (20 * 1000 * 1000);
   mb_slave_shutdown (slave);
#endif
}
