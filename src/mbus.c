/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2011 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#ifdef UNIT_TEST
#define mb_pdu_tx mock_mb_pdu_tx
#define mb_pdu_rx mock_mb_pdu_rx
#endif

#include "mbus.h"
#include "mb_pdu.h"
#include "mb_crc.h"
#include "osal.h"

#include <stdlib.h>
#include <errno.h>
#include <assert.h>
#include <string.h>

static int mb_is_exception (pdu_response_t * pdu)
{
   return pdu->function & BIT (7);
}

static int mb_exception (pdu_exception_t * pdu)
{
   switch (pdu->code)
   {
   case 1:
      return EILLEGAL_FUNCTION;
   case 2:
      return EILLEGAL_DATA_ADDRESS;
   case 3:
      return EILLEGAL_DATA_VALUE;
   case 4:
      return ESLAVE_DEVICE_FAILURE;
   default:
      return EUNKNOWN_EXCEPTION;
   }
}

int mbus_read (
   mbus_t * mbus,
   int slave,
   mb_address_t address,
   uint16_t quantity,
   void * buffer)
{
   pdu_txn_t * transaction = &mbus->transaction;
   pdu_read_t * request    = mbus->scratch;
   void * response         = mbus->scratch;
   int rx_count            = 0;
   int result;
   int i;

   if (slave == 0)
   {
      /* Broadcast read is not possible */
      return -1;
   }

   /* Build request */
   switch (address >> 16)
   {
   case 0:
      if (quantity > 2000)
      {
         return -1;
      }
      request->function = PDU_READ_COILS;
      break;
   case 1:
      if (quantity > 2000)
      {
         return -1;
      }
      request->function = PDU_READ_INPUTS;
      break;
   case 3:
      if (quantity > 125)
      {
         return -1;
      }
      request->function = PDU_READ_INPUT_REGISTERS;
      break;
   case 4:
      if (quantity > 125)
      {
         return -1;
      }
      request->function = PDU_READ_HOLDING_REGISTERS;
      break;
   default:
      return -1;
   }
   request->address  = CC_TO_BE16 ((address - 1) & 0xFFFF);
   request->quantity = CC_TO_BE16 (quantity);

   transaction->arg  = slave; /* ? */
   transaction->data = mbus->scratch;
   transaction->unit = slave;
   transaction->id++;

   /* Send request, receive response */
   mb_pdu_tx (mbus->transport, transaction, sizeof (*request));
   rx_count = mb_pdu_rx (mbus->transport, transaction, mbus->timeout);

   if (rx_count < 0)
   {
      result = rx_count;
   }
   else if (mb_is_exception (response))
   {
      result = mb_exception (response);
   }
   else
   {
      pdu_read_response_t * read_response = response;
      switch (read_response->function)
      {
      case PDU_READ_COILS: /* Fall-through */
      case PDU_READ_INPUTS:
         memcpy (buffer, read_response->data, read_response->count);
         result = 0;
         break;
      case PDU_READ_INPUT_REGISTERS: /* Fall-through */
      case PDU_READ_HOLDING_REGISTERS:
         for (i = 0; i < read_response->count; i += 2)
         {
            ((uint8_t *)buffer)[i + 1] = read_response->data[i];
            ((uint8_t *)buffer)[i]     = read_response->data[i + 1];
         }
         result = 0;
         break;
      default:
         result = -1;
         break;
      }
   }

   return result;
}

int mbus_write_single (
   mbus_t * mbus,
   int slave,
   mb_address_t address,
   uint16_t value)
{
   pdu_txn_t * transaction      = &mbus->transaction;
   pdu_write_single_t * request = mbus->scratch;
   void * response              = mbus->scratch;
   int rx_count                 = 0;
   int result;

   /* Build request */
   switch (address >> 16)
   {
   case 0:
      request->function = PDU_WRITE_COIL;
      value             = (value != 0) ? 0xFF00 : 0x0000;
      break;
   case 4:
      request->function = PDU_WRITE_HOLDING_REGISTER;
      break;
   default:
      return -1;
   }
   request->address = CC_TO_BE16 ((address - 1) & 0xFFFF);
   request->value   = CC_TO_BE16 (value);

   transaction->arg  = slave; /* ? */
   transaction->data = mbus->scratch;
   transaction->unit = slave;
   transaction->id++;

   /* Send request, receive response */
   mb_pdu_tx (mbus->transport, transaction, sizeof (*request));

   if (slave != 0)
   {
      rx_count = mb_pdu_rx (mbus->transport, transaction, mbus->timeout);
   }

   if (rx_count < 0)
   {
      result = rx_count;
   }
   else if (mb_is_exception (response))
   {
      result = mb_exception (response);
   }
   else
   {
      result = 0;
   }

   return result;
}

int mbus_write (
   mbus_t * mbus,
   int slave,
   mb_address_t address,
   uint16_t quantity,
   void * buffer)
{
   pdu_txn_t * transaction = &mbus->transaction;
   pdu_write_t * request   = mbus->scratch;
   void * response         = mbus->scratch;
   int rx_count            = 0;
   int result;
   uint8_t count = 0;
   int i;

   /* Build request */
   switch (address >> 16)
   {
   case 0:
      if (quantity > 1968)
      {
         return -1;
      }
      request->function = PDU_WRITE_COILS;
      count             = quantity / 8;
      if (quantity % 8 != 0)
      {
         count++;
      }
      memcpy (request->data, buffer, count);
      break;
   case 4:
      if (quantity > 123)
      {
         return -1;
      }
      request->function = PDU_WRITE_HOLDING_REGISTERS;
      count             = 2 * quantity;
      for (i = 0; i < count; i += 2)
      {
         request->data[i]     = ((uint8_t *)buffer)[i + 1];
         request->data[i + 1] = ((uint8_t *)buffer)[i];
      }
      break;
   default:
      return -1;
   }
   request->address  = CC_TO_BE16 ((address - 1) & 0xFFFF);
   request->quantity = CC_TO_BE16 (quantity);
   request->count    = count;

   transaction->arg  = slave; /* ? */
   transaction->data = mbus->scratch;
   transaction->unit = slave;
   transaction->id++;

   /* Send request, receive response */
   mb_pdu_tx (mbus->transport, transaction, (sizeof (pdu_write_t) + count));

   if (slave != 0)
   {
      rx_count = mb_pdu_rx (mbus->transport, transaction, mbus->timeout);
   }

   if (rx_count < 0)
   {
      result = rx_count;
   }
   else if (mb_is_exception (response))
   {
      result = mb_exception (response);
   }
   else
   {
      result = 0;
   }

   return result;
}

int mbus_loopback (mbus_t * mbus, int slave, uint16_t size, void * buffer)
{
   pdu_txn_t * transaction = &mbus->transaction;
   pdu_diag_t * request    = mbus->scratch;
   void * response         = mbus->scratch;
   int rx_count            = 0;
   int result;

   if (size > 250)
   {
      return -1;
   }

   /* Build request */
   request->function     = PDU_DIAGNOSTICS;
   request->sub_function = PDU_DIAG_LOOPBACK;

   memcpy (request->data, buffer, size);

   transaction->arg  = slave; /* ? */
   transaction->data = mbus->scratch;
   transaction->unit = slave;
   transaction->id++;

   /* Send request, receive response */
   mb_pdu_tx (mbus->transport, transaction, size + sizeof (*request));
   if (slave != 0)
   {
      rx_count = mb_pdu_rx (mbus->transport, transaction, mbus->timeout);
   }

   if (rx_count < 0)
   {
      result = rx_count;
   }
   else if (mb_is_exception (response))
   {
      result = mb_exception (response);
   }
   else
   {
      memcpy (buffer, response, (rx_count > size) ? size : rx_count);
      result = rx_count;
   }

   return result;
}

int mbus_send_msg (mbus_t * mbus, int slave, const void * msg, uint8_t size)
{
   pdu_txn_t * transaction = &mbus->transaction;

   transaction->arg  = slave; /* ? */
   transaction->data = (void *)msg;
   transaction->unit = slave;
   transaction->id++;

   mb_pdu_tx (mbus->transport, transaction, size);

   return 0;
}

int mbus_get_msg (mbus_t * mbus, int slave, void * msg, uint16_t size)
{
   pdu_txn_t * transaction = &mbus->transaction;
   int rx_count;

   transaction->arg  = slave; /* ? */
   transaction->data = msg;
   transaction->unit = slave;

   rx_count = mb_pdu_rx (mbus->transport, transaction, mbus->timeout);

   return rx_count;
}

void * mbus_transport_get (mbus_t * mbus)
{
   return mbus->transport;
}

int mbus_connect (mbus_t * mbus, const char * name)
{
   return mb_transport_bringup (mbus->transport, name);
}

int mbus_disconnect (mbus_t * mbus, int slave)
{
   return mb_transport_shutdown (mbus->transport, slave);
}

void mbus_init (
   mbus_t * mbus,
   const mbus_cfg_t * cfg,
   mb_transport_t * transport,
   uint8_t * scratch)
{
   mbus->timeout        = cfg->timeout;
   mbus->transaction.id = 0;
   mbus->scratch        = scratch;

   memset (mbus->scratch, 0x55, MAX_PDU_SIZE);

   /* Set transport layer */
   mbus->transport      = transport;
   transport->is_server = false;
}

mbus_t * mbus_create (const mbus_cfg_t * cfg, mb_transport_t * transport)
{
   mbus_t * mbus;
   uint8_t * scratch;

   /* Allocate and initialise driver structure */
   mbus = malloc (sizeof (mbus_t));
   CC_ASSERT (mbus != NULL);

   /* Allocate scratch buffer */
   scratch = malloc (MAX_PDU_SIZE);
   CC_ASSERT (scratch != NULL);

   mbus_init (mbus, cfg, transport, scratch);
   return mbus;
}
