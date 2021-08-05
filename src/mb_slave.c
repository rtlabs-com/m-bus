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

#ifdef UNIT_TEST
#define mb_pdu_tx       mock_mb_pdu_tx
#define mb_pdu_rx       mock_mb_pdu_rx
#define mb_pdu_rx_avail mock_mb_pdu_rx_avail
#define mb_pdu_rx_bc    mock_mb_pdu_rx_bc
#endif

#include "mb_slave.h"
#include "mb_transport.h"
#include "mb_pdu.h"
#include "mb_crc.h"
#include "osal.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>

#define PDU_TIMEOUT 5000 /* Timeout for rx retries (ticks) */

int mb_slave_bit_get (void * data, uint32_t address)
{
   uint32_t ix = address / 8;
   uint32_t offset = address % 8;
   uint8_t * p = data;

   return (p[ix] & BIT (offset)) ? 1 : 0;
}

void mb_slave_bit_set (void * data, uint32_t address, int value)
{
   uint32_t ix = address / 8;
   uint32_t offset = address % 8;
   uint8_t * p = data;

   if (value)
      p[ix] |= BIT (offset);
   else
      p[ix] &= ~BIT (offset);
}

uint16_t mb_slave_reg_get (void * data, uint32_t address)
{
   uint8_t * p = (uint8_t *)data + address * sizeof (uint16_t);
   return (p[0] << 8) | p[1];
}

void mb_slave_reg_set (void * data, uint32_t address, uint16_t value)
{
   uint8_t * p = (uint8_t *)data + address * sizeof (uint16_t);

   p[0] = value >> 8;
   p[1] = value & 0xFF;
}

static int mb_slave_read_bits (
   mb_transport_t * transport,
   const mb_iotable_t * iotable,
   pdu_t * pdu)
{
   pdu_read_t * request = &pdu->read;
   pdu_read_response_t * response = &pdu->read_response;
   uint8_t * pData = response->data;
   uint16_t address;
   uint16_t quantity;
   uint8_t count;
   int error;
   int ix;

   address = CC_FROM_BE16 (request->address);
   quantity = CC_FROM_BE16 (request->quantity);

   count = quantity / 8;
   if (quantity != count * 8)
      count++;

   if (quantity == 0 || quantity > 0x7D0)
      return EILLEGAL_DATA_VALUE;

   if (address + quantity > iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (iotable->get == NULL)
      return EILLEGAL_FUNCTION;

   /* Build response */
   response->count = count;

   error = iotable->get (address, pData, quantity);
   if (error)
      return error;

   /* Reset pad bits */
   for (ix = quantity; ix < count * 8; ix++)
   {
      mb_slave_bit_set (pData, ix, 0);
   }

   return sizeof (*response) + response->count;
}

static int mb_slave_read_registers (
   mb_transport_t * transport,
   const mb_iotable_t * iotable,
   pdu_t * pdu)
{
   pdu_read_t * request = &pdu->read;
   pdu_read_response_t * response = &pdu->read_response;
   uint8_t * pData = response->data;
   uint16_t address;
   uint16_t quantity;
   int error;

   address = CC_FROM_BE16 (request->address);
   quantity = CC_FROM_BE16 (request->quantity);

   if (quantity == 0 || quantity > 0x7D)
      return EILLEGAL_DATA_VALUE;

   if (address + quantity > iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (iotable->get == NULL)
      return EILLEGAL_FUNCTION;

   /* Build response */
   response->count = 2 * quantity;

   error = iotable->get (address, pData, quantity);
   if (error)
      return error;

   return sizeof (*response) + response->count;
}

static int mb_slave_write_bit (
   mb_transport_t * transport,
   const mb_iotable_t * iotable,
   pdu_t * pdu)
{
   pdu_write_single_t * request = &pdu->write_single;
   uint16_t address;
   uint16_t value;
   uint8_t bit;
   int error;

   address = CC_FROM_BE16 (request->address);
   value = CC_FROM_BE16 (request->value);

   if (value != 0 && value != 0xFF00)
      return EILLEGAL_DATA_VALUE;

   if (address >= iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (iotable->set == NULL)
      return EILLEGAL_FUNCTION;

   bit = (value == 0xFF00) ? 1 : 0;

   error = iotable->set (address, &bit, 1);
   if (error)
      return error;

   /* Echo request */
   return sizeof (pdu_write_single_response_t);
}

static int mb_slave_write_bits (
   mb_transport_t * transport,
   const mb_iotable_t * iotable,
   pdu_t * pdu,
   size_t rx_count)
{
   pdu_write_t * request = &pdu->write;
   uint8_t * pData = request->data;
   uint16_t address;
   uint16_t quantity;
   uint8_t count;
   int error;

   address = CC_FROM_BE16 (request->address);
   quantity = CC_FROM_BE16 (request->quantity);

   count = quantity / 8;
   if (quantity != count * 8)
      count++;

   if (quantity == 0 || quantity > 0x7B0)
      return EILLEGAL_DATA_VALUE;

   if (count != request->count)
      return EILLEGAL_DATA_VALUE;

   if (address + quantity > iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (rx_count != sizeof (*request) + request->count)
      return EILLEGAL_DATA_VALUE;

   if (iotable->set == NULL)
      return EILLEGAL_FUNCTION;

   error = iotable->set (address, pData, quantity);
   if (error)
      return error;

   /* Echo request */
   return sizeof (pdu_write_response_t);
}

static int mb_slave_write_register (
   mb_transport_t * transport,
   const mb_iotable_t * iotable,
   pdu_t * pdu)
{
   pdu_write_single_t * request = &pdu->write_single;
   uint16_t address;
   int error;

   address = CC_FROM_BE16 (request->address);

   if (address >= iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (iotable->set == NULL)
      return EILLEGAL_FUNCTION;

   error = iotable->set (address, (uint8_t *)&request->value, 1);
   if (error)
      return error;

   /* Echo request */
   return sizeof (pdu_write_single_response_t);
}

static int mb_slave_write_registers (
   mb_transport_t * transport,
   const mb_iotable_t * iotable,
   pdu_t * pdu,
   size_t rx_count)
{
   pdu_write_t * request = &pdu->write;
   uint8_t * pData = request->data;
   uint16_t address;
   uint16_t quantity;
   int error;

   address = CC_FROM_BE16 (request->address);
   quantity = CC_FROM_BE16 (request->quantity);

   if (quantity == 0 || quantity > 0x7B)
      return EILLEGAL_DATA_VALUE;

   if (2 * quantity != request->count)
      return EILLEGAL_DATA_VALUE;

   if (address + quantity > iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (rx_count != sizeof (*request) + request->count)
      return EILLEGAL_DATA_VALUE;

   if (iotable->set == NULL)
      return EILLEGAL_FUNCTION;

   error = iotable->set (address, pData, quantity);
   if (error)
      return error;

   /* Echo request */
   return sizeof (pdu_write_response_t);
}

static int mb_slave_read_write_registers (
   mb_transport_t * transport,
   const mb_iotable_t * iotable,
   pdu_t * pdu,
   size_t rx_count)
{
   pdu_read_write_t * request = &pdu->read_write;
   pdu_read_response_t * response = &pdu->read_response;
   uint16_t write_address;
   uint16_t write_quantity;
   uint16_t read_address;
   uint16_t read_quantity;
   int error;

   write_address = CC_FROM_BE16 (request->write_address);
   write_quantity = CC_FROM_BE16 (request->write_quantity);

   if (write_quantity == 0 || write_quantity > 0x79)
      return EILLEGAL_DATA_VALUE;

   if (2 * write_quantity != request->count)
      return EILLEGAL_DATA_VALUE;

   if (write_address + write_quantity > iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (rx_count != sizeof (*request) + request->count)
      return EILLEGAL_DATA_VALUE;

   if (iotable->set == NULL)
      return EILLEGAL_FUNCTION;

   read_address = CC_FROM_BE16 (request->read_address);
   read_quantity = CC_FROM_BE16 (request->read_quantity);

   if (read_quantity == 0 || read_quantity > 0x7D)
      return EILLEGAL_DATA_VALUE;

   if (read_address + read_quantity > iotable->size)
      return EILLEGAL_DATA_ADDRESS;

   if (iotable->get == NULL)
      return EILLEGAL_FUNCTION;

   /* Perform write */
   error = iotable->set (write_address, request->data, write_quantity);
   if (error)
      return error;

   /* Build response */
   response->count = 2 * read_quantity;
   error = iotable->get (read_address, response->data, read_quantity);
   if (error)
      return error;

   return sizeof (*response) + response->count;
}

static int mb_slave_diagnostics (
   mb_transport_t * transport,
   pdu_t * pdu,
   size_t rx_count)
{
   pdu_diag_t * request = &pdu->diag;
   uint16_t sub_function;

   sub_function = CC_FROM_BE16 (request->sub_function);
   switch (sub_function)
   {
   case PDU_DIAG_LOOPBACK:
      return (int)rx_count;
   default:
      break;
   }

   return EILLEGAL_FUNCTION;
}

static int mb_slave_vendor (
   mb_transport_t * transport,
   const mb_iomap_t * iomap,
   pdu_t * pdu,
   size_t rx_count)
{
   pdu_vendor_t * request = &pdu->vendor;
   size_t i;
   int tx_count;

   if (iomap->num_vendor_funcs == 0 || iomap->vendor_funcs == NULL)
      return EILLEGAL_FUNCTION;

   /* Find matching vendor-defined function */
   for (i = 0; i < iomap->num_vendor_funcs; i++)
   {
      if (iomap->vendor_funcs[i].function == request->function)
      {
         tx_count =
            iomap->vendor_funcs[i].callback (&request->function, rx_count);

         return tx_count;
      }
   }

   return EILLEGAL_FUNCTION;
}

void mb_slave_handle_request (mb_slave_t * slave, pdu_txn_t * transaction)
{
   mb_transport_t * transport = slave->transport;
   pdu_t * pdu = transaction->data;
   int rx_count;
   int tx_count = 0;

   /* Wait for incoming request. Set a timeout to be able to retry
      the call if the slave ID changes. */
   rx_count = mb_pdu_rx (transport, transaction, PDU_TIMEOUT);

   if (rx_count > 0)
   {
      switch (pdu->request.function)
      {
      case PDU_READ_COILS:
         if (!mb_pdu_rx_bc (transport))
         {
            tx_count =
               mb_slave_read_bits (transport, &slave->iomap->coils, pdu);
         }
         break;
      case PDU_READ_INPUTS:
         if (!mb_pdu_rx_bc (transport))
         {
            tx_count =
               mb_slave_read_bits (transport, &slave->iomap->inputs, pdu);
         }
         break;
      case PDU_READ_INPUT_REGISTERS:
         if (!mb_pdu_rx_bc (transport))
         {
            tx_count = mb_slave_read_registers (
               transport,
               &slave->iomap->input_registers,
               pdu);
         }
         break;
      case PDU_READ_HOLDING_REGISTERS:
         if (!mb_pdu_rx_bc (transport))
         {
            tx_count = mb_slave_read_registers (
               transport,
               &slave->iomap->holding_registers,
               pdu);
         }
         break;
      case PDU_WRITE_COIL:
         tx_count = mb_slave_write_bit (transport, &slave->iomap->coils, pdu);
         break;
      case PDU_WRITE_HOLDING_REGISTER:
         tx_count = mb_slave_write_register (
            transport,
            &slave->iomap->holding_registers,
            pdu);
         break;
      case PDU_WRITE_COILS:
         tx_count =
            mb_slave_write_bits (transport, &slave->iomap->coils, pdu, rx_count);
         break;
      case PDU_WRITE_HOLDING_REGISTERS:
         tx_count = mb_slave_write_registers (
            transport,
            &slave->iomap->holding_registers,
            pdu,
            rx_count);
         break;
      case PDU_READ_WRITE_HOLDING_REGISTERS:
         tx_count = mb_slave_read_write_registers (
            transport,
            &slave->iomap->holding_registers,
            pdu,
            rx_count);
         break;
      case PDU_DIAGNOSTICS:
         tx_count = mb_slave_diagnostics (transport, pdu, rx_count);
         break;
      default:
         tx_count = mb_slave_vendor (transport, slave->iomap, pdu, rx_count);
         break;
      }

      /* Check for exception */
      if (tx_count < 0)
      {
         pdu_exception_t * exception = &pdu->exception;

         exception->function |= BIT (7);
         exception->code = -tx_count;

         tx_count = sizeof (*exception);
      }

      /* Respond only if no further messages have appeared on bus */
      if (mb_pdu_rx_avail (transport))
      {
         /* The master has timed out and sent a new request. We
          * have no way of knowing when this request arrived; the
          * master may have timed out waiting for a response to
          * this request also. To be safe we ignore this request.
          */

         rx_count = mb_pdu_rx (transport, transaction, 0);
         (void)rx_count;
      }
      /* No response to broadcast messages. */
      else if (!mb_pdu_rx_bc (transport))
      {
         /* Send response */
         mb_pdu_tx (transport, transaction, tx_count);
      }
   }
}

static void mb_slave (void * arg)
{
   mb_slave_t * slave = arg;
   mb_transport_t * transport = slave->transport;
   pdu_txn_t transaction;
   uint8_t buffer[MAX_PDU_SIZE];

   memset (buffer, 0x55, sizeof (buffer));

   transaction.data = buffer;
   transaction.arg = 0;

   while (slave->running)
   {
      while (mb_transport_is_down (transport))
      {
         transaction.arg = mb_transport_bringup (transport, NULL);
         if (transaction.arg == -1)
            break;
      }

      if (transaction.arg == -1)
         continue;

      transaction.unit = slave->id;

      mb_slave_handle_request (slave, &transaction);
   }
}

void * mb_slave_transport_get (mb_slave_t * slave)
{
   return slave->transport;
}

void mb_slave_id_set (mb_slave_t * slave, uint8_t id)
{
   slave->id = id;
}

void mb_slave_shutdown (mb_slave_t * slave)
{
   slave->running = 0;
}

mb_slave_t * mb_slave_init (
   const mb_slave_cfg_t * cfg,
   mb_transport_t * transport)
{
   mb_slave_t * slave;

   /* Allocate and initialise slave state */
   slave = malloc (sizeof (mb_slave_t));
   CC_ASSERT (slave != NULL);

   slave->iomap = cfg->iomap;

   /* Set transport layer */
   slave->transport = transport;
   transport->is_server = true;

   slave->id = cfg->id;
   slave->running = 1;

   /* Start slave task */
   os_thread_create (
      "tMbSlave",
      cfg->priority,
      cfg->stack_size,
      mb_slave,
      slave);

   return slave;
}
