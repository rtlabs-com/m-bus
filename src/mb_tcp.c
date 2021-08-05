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

#include "mb_tcp.h"
#include "mb_transport.h"
#include "mb_pdu.h"
#include "osal.h"
#include "mbal_tcp.h"
#include "osal_log.h"
#include "options.h"
#include "inttypes.h"

#include <stdlib.h>
#include <string.h>

#define KEEP_ALIVE_IDLE  10 /* max idle time before keepalive sent [s] */
#define KEEP_ALIVE_INTVL 2  /* time between keepalives [s] */
#define KEEP_ALIVE_CNT   3  /* max number of unacked keepalives */

#define RCV_TIMEOUT 500 /* max time to wait for message in progress [ms] */

typedef struct mbap
{
   uint16_t id;
   uint16_t protocol;
   uint16_t length;
   uint8_t unit;
   uint8_t data[MAX_PDU_SIZE];
} CC_PACKED mbap_t;

#define MBAP_HEADER_SIZE offsetof (mbap_t, data)

struct mb_tcp /* Typedef in mb_tcp.h */
{
   mb_transport_t transport;
   uint16_t port;
   bool is_down;
   mbap_t mbap;
};

static int mb_tcp_bringup (mb_transport_t * transport, const char * name)
{
   mb_tcp_t * mb_tcp = (mb_tcp_t *)transport;
   int peer;

   if (transport->is_server)
   {
      peer = os_tcp_accept_connection (mb_tcp->port);
   }
   else
   {
      peer = os_tcp_connect (name, mb_tcp->port);
   }

   if (peer > 0)
   {
      mb_tcp->is_down = false;
      LOG_INFO (MB_TCP_LOG, "Connection established\n");
   }

   return peer;
}

static int mb_tcp_shutdown (mb_transport_t * transport, int arg)
{
   mb_tcp_t * mb_tcp = (mb_tcp_t *)transport;
   int peer          = arg;

   if (mb_tcp->is_down == false)
   {
      LOG_INFO (MB_TCP_LOG, "Connection closed\n");
      os_tcp_close (peer);
      mb_tcp->is_down = true;
   }

   os_usleep (10 * 1000);
   return 0;
}

static bool mb_tcp_is_down (mb_transport_t * transport)
{
   mb_tcp_t * mb_tcp = (mb_tcp_t *)transport;
   return mb_tcp->is_down;
}

static void mb_tcp_tx (
   mb_transport_t * transport,
   const pdu_txn_t * transaction,
   size_t size)
{
   mb_tcp_t * mb_tcp = (mb_tcp_t *)transport;
   int peer          = transaction->arg;
   mbap_t * mbap     = &mb_tcp->mbap;
   ssize_t result;

   mbap->id       = CC_TO_BE16 (transaction->id);
   mbap->length   = CC_TO_BE16 ((uint16_t)size + 1); /* Includes size of unit id */
   mbap->protocol = 0;
   mbap->unit     = transaction->unit;

   memcpy (mbap->data, transaction->data, size);
   result = os_tcp_send (peer, mbap, MBAP_HEADER_SIZE + size);
   LOG_DEBUG (MB_TCP_LOG, "Sent mbap\n");

   if (result <= 0)
   {
      /* Peer closed their connection or some other error. Close
         connection. */
      LOG_INFO (MB_TCP_LOG, "Connection closed\n");
      os_tcp_close (peer);
      mb_tcp->is_down = true;
      return;
   }
}

static int mb_tcp_rx (
   mb_transport_t * transport,
   pdu_txn_t * transaction,
   uint32_t tmo)
{
   mb_tcp_t * mb_tcp = (mb_tcp_t *)transport;
   int peer          = transaction->arg;
   mbap_t * mbap     = &mb_tcp->mbap;
   size_t size       = 0;
   ssize_t result;

   /* Wait for next message until timeout */
   result = os_tcp_recv_wait (peer, tmo);
   if (result == -1)
   {
      LOG_INFO (MB_TCP_LOG, "Connection closed\n");
      os_tcp_close (peer);
      mb_tcp->is_down = true;
      return EFRAME_NOK;
   }
   if (result == 0)
   {
      /* Timeout */
      return ETIMEOUT;
   }

   /* Get message. The recv function will timeout if data is not
      available in a reasonable timeframe. */

   LOG_DEBUG (MB_TCP_LOG, "Getting header\n");
   result = os_tcp_recv (peer, mbap, MBAP_HEADER_SIZE);
   if (result == MBAP_HEADER_SIZE)
   {
      /* The size of the PDU includes the unit id we already read */
      size = CC_FROM_BE16 (mbap->length) - 1;

      /* Never overflow buffer */
      if (size > MAX_PDU_SIZE)
         size = MAX_PDU_SIZE;

      LOG_DEBUG (MB_TCP_LOG, "Getting %d bytes\n", (unsigned)size);
      result = os_tcp_recv (peer, transaction->data, size);
   }

   if (result <= 0)
   {
      /* Peer closed their connection or some other error. Drop
         message, close connection. */
      LOG_INFO (MB_TCP_LOG, "Connection closed\n");
      os_tcp_close (peer);
      mb_tcp->is_down = true;
      return EFRAME_NOK;
   }

   /* Drop message if protocol field invalid */
   if (mbap->protocol != 0)
   {
      return EFRAME_NOK;
   }

   transaction->id   = CC_FROM_BE16 (mbap->id);
   transaction->unit = mbap->unit;

   return (int)size;
}

static bool mb_tcp_rx_is_bc (mb_transport_t * transport)
{
   /* No broadcasts in Modbus/TCP */
   return false;
}

static bool mb_tcp_rx_avail (mb_transport_t * transport)
{
   /* This function is used to avoid sending a reply if another
      request has been received. In the TCP case the transaction ID is
      used to differentiate between replies so we can always send the
      reply. */
   return false;
}

mb_transport_t * mb_tcp_init (const mb_tcp_cfg_t * cfg)
{
   mb_tcp_t * mb_tcp;

   /* Allocate and initialise driver structure */

   mb_tcp = malloc (sizeof (mb_tcp_t));
   CC_ASSERT (mb_tcp != NULL);

   mb_tcp->transport.bringup  = mb_tcp_bringup;
   mb_tcp->transport.shutdown = mb_tcp_shutdown;
   mb_tcp->transport.is_down  = mb_tcp_is_down;
   mb_tcp->transport.tx       = mb_tcp_tx;
   mb_tcp->transport.rx       = mb_tcp_rx;
   mb_tcp->transport.rx_is_bc = mb_tcp_rx_is_bc;
   mb_tcp->transport.rx_avail = mb_tcp_rx_avail;

   mb_tcp->is_down = true;
   mb_tcp->port    = cfg->port;

   return (mb_transport_t *)mb_tcp;
}
