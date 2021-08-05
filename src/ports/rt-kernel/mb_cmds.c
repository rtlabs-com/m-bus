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

#include <shell.h>
#include <stdio.h>

#include "mb_master.h"

static int _cmd_mb_read (int argc, char * argv[])
{
   uint32_t reg;
   uint16_t value = 0;
   int fd;
   int result;

   if (argc != 3)
   {
      shell_usage (argv[0], "wrong number of arguments");
      return -1;
   }

   reg = strtoul (argv[2], NULL, 0);
   if (reg > 0x4FFFF)
   {
      printf ("Bad register\n");
      return -1;
   }

   fd = mb_open (argv[1]);
   if (fd < 0)
   {
      printf ("Failed to open %s\n", argv[1]);
      return -1;
   }

   result = mb_read (fd, reg, 1, &value);
   if (result != 0)
   {
      printf ("Failed to read value (result %s)\n", mb_error_literal (result));
      mb_close (fd);
      return -1;
   }

   printf ("0x%04x\n", value);

   mb_close (fd);
   return 0;
}

static int _cmd_mb_write (int argc, char * argv[])
{
   uint32_t reg;
   uint16_t value;
   int fd;
   int result;

   if (argc != 4)
   {
      shell_usage (argv[0], "wrong number of arguments");
      return -1;
   }

   reg = strtoul (argv[2], NULL, 0);
   if (reg > 0x4FFFF)
   {
      printf ("Bad register\n");
      return -1;
   }

   value = strtoul (argv[3], NULL, 0) & 0xFFFF;

   fd = mb_open (argv[1]);
   if (fd < 0)
   {
      printf ("Failed to open %s\n", argv[1]);
      return -1;
   }

   result = mb_write (fd, reg, 1, &value);
   if (result != 0)
   {
      printf ("Failed to write value (result %s)\n", mb_error_literal (result));
      mb_close (fd);
      return -1;
   }

   mb_close (fd);
   return 0;
}

const shell_cmd_t cmd_mb_read = {
   .cmd = _cmd_mb_read,
   .name = "mb_read",
   .help_short = "modbus read",
   .help_long =

      "mb_read name reg\n"
      "\n"
      "Read from modbus slave. The modbus slave is specified through "
      "the\n"
      "name, e.g:\n"
      "\n"
      "/modbus0/15                 (RTU slave 15)\n"
      "/modbus1/192.168.10.101     (TCP slave 192.168.10.101)\n"
      "\n"
      "The register is specified using the format:\n"
      "\n"
      "    0xTRRRR\n"
      "\n"
      "where T is:\n"
      "\n"
      "  0 Coils\n"
      "  1 Inputs\n"
      "  3 Input registers\n"
      "  4 Holding registers\n"
      "\n"
      "and RRRR is the register address, starting at 1. Example:\n"
      "\n"
      "> mb_read /modbus0/192.168.10.101 0x40001\n"
      "(read holding register 1 in modbus TCP slave 192.168.10.101 "
      "on bus\n"
      "modbus0).\n"};

const shell_cmd_t cmd_mb_write = {
   .cmd = _cmd_mb_write,
   .name = "mb_write",
   .help_short = "modbus write",
   .help_long =

      "mb_write name reg value\n"
      "\n"
      "Write to modbus slave. The modbus slave is specified through "
      "the\n"
      "name, e.g:\n"
      "\n"
      "/modbus0/15                 (RTU slave 15)\n"
      "/modbus1/192.168.10.101     (TCP slave 192.168.10.101)\n"
      "\n"
      "The register is specified using the format:\n"
      "\n"
      "    0xTRRRR\n"
      "\n"
      "where T is:\n"
      "\n"
      "  0 Coils\n"
      "  1 Inputs\n"
      "  3 Input registers\n"
      "  4 Holding registers\n"
      "\n"
      "and RRRR is the register address, starting at 1. Example:\n"
      "\n"
      "> mb_write /modbus0/192.168.10.101 0x40001 0x1234\n"
      "(write holding register 1 in modbus TCP slave 192.168.10.101 "
      "on bus\n"
      "modbus0).\n"};
