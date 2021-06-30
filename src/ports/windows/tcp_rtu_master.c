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

#include "mbus.h"
#include "mb_tcp.h"
#include "mb_rtu.h"
#include "mb_bsp.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include <windows.h>

static struct opt
{
   int repeat;
   int delay;
   bool read;
   bool write;
   bool tcp;
   bool rtu;
   struct
   {
      char ip[20];
      mb_tcp_cfg_t cfg;
   } tcp_details;
   struct
   {
      char device[80];
      char unit[4];
      mb_rtu_serial_cfg_t cfg;
   } rtu_details;
   int table;
   uint32_t address;
   uint32_t value;
} opt;

static void help (const char * name)
{
   printf (
      "Read/write Modbus registers\n"
      "\n"
      "This purpose of this program is to demonstrate use of the m-bus\n"
      "Modbus master stack. It may also be useful as a debugging tool.\n"
      "\n"
      "USAGE:\n"
      "%s [OPTIONS] cmd transport connection table address [value]\n"
      "\n"
      "cmd         {read, write}            Type of transaction\n"
      "transport   {tcp, rtu}               Type of transport\n"
      "connection  {IP:PORT, DEVICE:BAUDRATE:PARITY:UNIT}\n"
      "                                     Connection details\n"
      "table       {coil, input, reg, hold} Modbus address table\n"
      "address                              Modbus address\n"
      "\n"
      "TCP CONNECTION DETAILS\n"
      "IP          IP-address, e.g. 192.168.0.1\n"
      "PORT        Modbus port, e.g. 502\n"
      "\n"
      "RTU CONNECTION DETAILS\n"
      "DEVICE      Serial device, e.g. /dev/ttyUSB0\n"
      "BAUDRATE    Modbus baudrate\n"
      "PARITY      Modbus parity {odd, even, none}\n"
      "\n"
      "OPTIONS:\n"
      " --repeat n Repeat command n times\n"
      " --delay ms Time to delay between Modbus transactions\n"
      "\n"
      "EXAMPLES:\n"
      "\n"
      "Repeatedly read coil 1 of Modbus TCP slave 192.168.10.134, port 8502.\n"
      "\n"
      "%s --repeat 100 read tcp 192.168.10.134:8502 coil 0x0001\n"
      "\n"
      "Write 42 to holding register 3 of Modbus RTU slave 2, accessed\n"
      "through /dev/ttyUSB0 with baudrate 115200, no parity.\n"
      "\n"
      "%s write rtu /dev/ttyUSB0:115200:none:2 hold 0x0003 42\n"
      "\n",
      name,
      name,
      name);
   exit (EXIT_SUCCESS);
}

static void fail (const char * name, const char * reason)
{
   if (reason != NULL)
   {
      printf ("%s. ", reason);
   }
   printf ("Try %s --help\n", name);
   exit (EXIT_FAILURE);
}

static bool parse_tcp_details (char * s)
{
   char * p;
   char * saveptr;

   /* Get ip address */

   p = strtok_s (s, ":", &saveptr);
   if (p == NULL)
      return false;

   strncpy_s (
      opt.tcp_details.ip,
      sizeof (opt.tcp_details.ip),
      p,
      sizeof (opt.tcp_details.ip));
   opt.tcp_details.ip[sizeof (opt.tcp_details.ip) - 1] = 0;

   /* Get ip port */

   p = strtok_s (NULL, ":", &saveptr);
   if (p == NULL)
      return false;

   opt.tcp_details.cfg.port = (uint16_t)strtoul (p, NULL, 0);

   /* Check for extra unknown options */

   p = strtok_s (NULL, ":", &saveptr);
   if (p != NULL)
      return false;

   return true;
}

static bool parse_rtu_details (char * s)
{
   char * p;
   char * saveptr;

   /* Get device */

   p = strtok_s (s, ":", &saveptr);
   if (p == NULL)
      return false;

   strncpy_s (
      opt.rtu_details.device,
      sizeof (opt.rtu_details.device),
      p,
      sizeof (opt.rtu_details.device));
   opt.rtu_details.device[sizeof (opt.rtu_details.device) - 1] = 0;

   /* Get baudrate */

   p = strtok_s (NULL, ":", &saveptr);
   if (p == NULL)
      return false;

   opt.rtu_details.cfg.baudrate = strtoul (p, NULL, 0);

   /* Get parity */

   p = strtok_s (NULL, ":", &saveptr);
   if (p == NULL)
      return false;

   if (strcmp (p, "odd") == 0)
      opt.rtu_details.cfg.parity = ODD;
   else if (strcmp (p, "even") == 0)
      opt.rtu_details.cfg.parity = EVEN;
   else if (strcmp (p, "none") == 0)
      opt.rtu_details.cfg.parity = NONE;
   else
      return false;

   /* Get unit */

   p = strtok_s (NULL, ":", &saveptr);
   if (p == NULL)
      return false;

   strncpy_s (
      opt.rtu_details.unit,
      sizeof (opt.rtu_details.unit),
      p,
      sizeof (opt.rtu_details.unit));
   opt.rtu_details.unit[sizeof (opt.rtu_details.unit) - 1] = 0;

   /* Check for extra unknown options */

   p = strtok_s (NULL, ":", &saveptr);
   if (p != NULL)
      return false;

   return true;
}

static void parse_opt (int argc, char * argv[])
{
   int optind = 1;

   /* Set defaults */
   opt.repeat = 1;

   while (optind < argc)
   {
      if (strcmp (argv[optind], "--help") == 0)
      {
         help (argv[0]);
         exit (EXIT_SUCCESS);
      }
      else if (strcmp (argv[optind], "--repeat") == 0)
      {
         opt.repeat = strtoul (argv[++optind], NULL, 0);
      }
      else if (strcmp (argv[optind], "--delay") == 0)
      {
         opt.delay = strtoul (argv[++optind], NULL, 0);
      }
      else if (argv[optind][0] == '-')
      {
         fail (argv[0], "unknown option");
      }
      else
      {
         break;
      }

      ++optind;
   }

   /* Get command */

   if (optind == argc)
      fail (argv[0], "Nothing to do");

   opt.read = strcmp (argv[optind], "read") == 0;
   opt.write = strcmp (argv[optind], "write") == 0;

   /* Get transport type */

   if (++optind == argc)
      fail (argv[0], "No transport");

   opt.tcp = strcmp (argv[optind], "tcp") == 0;
   opt.rtu = strcmp (argv[optind], "rtu") == 0;

   /* Get transport details */

   if (++optind == argc)
      fail (argv[0], "No transport details");

   if (opt.tcp)
   {
      if (!parse_tcp_details (argv[optind]))
         fail (argv[0], "Bad tcp details");
   }
   else if (opt.rtu)
   {
      if (!parse_rtu_details (argv[optind]))
         fail (argv[0], "Bad rtu details");
   }

   /* Get address table */

   if (++optind == argc)
      fail (argv[0], "No address table");

   if (strcmp (argv[optind], "coil") == 0)
      opt.table = 0;
   else if (strcmp (argv[optind], "input") == 0)
      opt.table = 1;
   else if (strcmp (argv[optind], "reg") == 0)
      opt.table = 3;
   else if (strcmp (argv[optind], "hold") == 0)
      opt.table = 4;
   else
      fail (argv[0], "Bad address table");

   /* Get register address */

   if (++optind == argc)
      fail (argv[0], "No address");

   opt.address = strtoul (argv[optind], NULL, 0);
   if (opt.address > 0x10000)
      fail (argv[0], "Bad address");

   if (opt.write)
   {
      /* Get value */
      if (++optind == argc)
         fail (argv[0], "No value");

      opt.value = strtoul (argv[optind], NULL, 0);
      if (opt.value > 0xFFFF)
         fail (argv[0], "Bad value");
   }

   /* Check for extra unknown options */

   if (++optind != argc)
      fail (argv[0], "Too many options");
}

int main (int argc, char * argv[])
{
   static mbus_cfg_t mb_master_cfg = {
      .timeout = 1000,
   };
   mbus_t * mbus;
   int slave;

   parse_opt (argc, argv);

   if (opt.tcp)
   {
      mb_transport_t * tcp;

      tcp = mb_tcp_init (&opt.tcp_details.cfg);
      mbus = mbus_create (&mb_master_cfg, tcp);

      slave = mbus_connect (mbus, opt.tcp_details.ip);
   }
   else if (opt.rtu)
   {
      mb_transport_t * rtu;

      rtu = mb_rtu_create (opt.rtu_details.device, &opt.rtu_details.cfg);
      mbus = mbus_create (&mb_master_cfg, rtu);

      slave = mbus_connect (mbus, opt.rtu_details.unit);
   }
   else
   {
      fail (argv[0], "Unknown transport");
   }

   if (slave == -1)
   {
      fail (argv[0], "Failed to open socket");
   }

   for (int i = 0; i < opt.repeat; i++)
   {
      uint16_t value = 0;
      int error = 0;
      mb_address_t address = MB_ADDRESS (opt.table, opt.address);

      if (opt.read)
      {
         error = mbus_read (mbus, slave, address, 1, &value);
      }
      else if (opt.write)
      {
         error = mbus_write_single (mbus, slave, address, opt.value);
      }
      else
      {
         fail (argv[0], "Unknown command");
      }

      if (error)
         printf ("Modbus function failed (%s).\n", mb_error_literal (error));
      else if (opt.read)
         printf ("0x%04x\n", value);

      Sleep (opt.delay);
   }
}
