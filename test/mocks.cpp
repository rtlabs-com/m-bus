/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2019 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "mocks.h"

#include <gtest/gtest.h>
#include <string.h>

unsigned int mock_mb_pdu_tx_calls;
pdu_txn_t mock_mb_pdu_tx_transaction;
uint8_t mock_mb_pdu_tx_data[MAX_PDU_SIZE];
uint8_t mock_mb_pdu_tx_size;

void mock_mb_pdu_tx (
   mb_transport_t * transport,
   const pdu_txn_t * transaction,
   size_t size)
{
   mock_mb_pdu_tx_calls++;
   mock_mb_pdu_tx_size = size;
   memset (mock_mb_pdu_tx_data, 0, sizeof (mock_mb_pdu_tx_data));
   memcpy (mock_mb_pdu_tx_data, transaction->data, size);
}

unsigned int mock_mb_pdu_rx_calls;
const uint8_t * mock_mb_pdu_rx_data;
uint8_t mock_mb_pdu_rx_size;
int mock_mb_pdu_rx_result;

int mock_mb_pdu_rx (
   mb_transport_t * transport,
   pdu_txn_t * transaction,
   uint32_t tmp)
{
   mock_mb_pdu_rx_calls++;
   memcpy (transaction->data, mock_mb_pdu_rx_data, mock_mb_pdu_rx_size);
   return mock_mb_pdu_rx_result;
}

bool mock_mb_pdu_rx_bc (mb_transport_t * transport)
{
   return false;
}

bool mock_mb_pdu_rx_avail (mb_transport_t * transport)
{
   return false;
}
