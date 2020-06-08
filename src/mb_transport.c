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

#include "mb_transport.h"

int mb_transport_bringup (mb_transport_t * transport, const char * name)
{
   return transport->bringup (transport, name);
}

int mb_transport_shutdown (mb_transport_t * transport, int arg)
{
   return transport->shutdown (transport, arg);
}

bool mb_transport_is_down (mb_transport_t * transport)
{
   return transport->is_down (transport);
}

void mb_pdu_tx (
   mb_transport_t * transport,
   const pdu_txn_t * transaction,
   size_t size)
{
   transport->tx (transport, transaction, size);
}

int mb_pdu_rx (mb_transport_t * transport, pdu_txn_t * transaction, uint32_t tmo)
{
   return transport->rx (transport, transaction, tmo);
}

bool mb_pdu_rx_bc (mb_transport_t * transport)
{
   return transport->rx_is_bc (transport);
}

bool mb_pdu_rx_avail (mb_transport_t * transport)
{
   return transport->rx_avail (transport);
}
