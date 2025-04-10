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

#include "mb_master.h"
#include "mb_bsp.h"

SHELL_CMD (cmd_mb_read);
SHELL_CMD (cmd_mb_write);

int main (void)
{
   static mb_master_cfg_t mb_master_cfg = {
      .timeout = 1000,
   };
   mb_transport_t * rtu;

   /* Configure RTU Modbus master */
   rtu = mb_rtu_create();
   mb_master_init ("/modbus0", &mb_master_cfg, rtu);
}
