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
#include "mb_rtu.h"
#include "osal.h"

#include <string.h>

static uint8_t coils[2] = {0x55, 0xAA};
static uint16_t hold[4] = {0x1234, 0x5678, 0x55AA, 0xAA55};

static int coil_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t bit = address + offset;
      int value;

      value = mb_slave_bit_get (coils, bit);
      mb_slave_bit_set (data, offset, value);
   }
   return 0;
}

static int coil_set (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t bit = address + offset;
      int value;

      value = mb_slave_bit_get (data, offset);
      mb_slave_bit_set (coils, bit, value);
   }
   return 0;
}

static int input_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      mb_slave_bit_set (data, offset, 1);
   }
   return 0;
}

static int hold_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t reg = address + offset;

      mb_slave_reg_set (data, offset, hold[reg]);
   }
   return 0;
}

static int hold_set (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t reg = address + offset;

      hold[reg] = mb_slave_reg_get (data, offset);
   }
   return 0;
}

static int reg_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      mb_slave_reg_set (data, offset, 0x1100 | (offset & 0xFF));
   }
   return 0;
}

static int ping (uint8_t * data, size_t rx_count)
{
   char * message = "Hello World";
   memcpy (data, message, strlen (message));
   return (int)strlen (message);
}

static const mb_vendor_func_t vendor_funcs[] = {
   {101, ping},
};

const mb_iomap_t mb_slave_iomap = {
   .coils             = {16, coil_get, coil_set}, // 16 coils
   .inputs            = {2, input_get, NULL},     // 2 input status bits
   .holding_registers = {4, hold_get, hold_set},  // 4 holding registers
   .input_registers   = {5, reg_get, NULL},       // 5 input registers
   .num_vendor_funcs  = NELEMENTS (vendor_funcs), // 1 vendor function
   .vendor_funcs      = vendor_funcs,
};

const mb_slave_cfg_t mb_slave_cfg = {
   .id         = 2, // Slave ID: 2
   .priority   = 15,
   .stack_size = 2048,
   .iomap      = &mb_slave_iomap};
