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

#ifndef MOCKS_H
#define MOCKS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

#include "mb_transport.h"
#include "mb_pdu.h"

extern unsigned int mock_mb_pdu_tx_calls;
extern pdu_txn_t mock_mb_pdu_tx_transaction;
extern uint8_t mock_mb_pdu_tx_data[MAX_PDU_SIZE];
extern uint8_t mock_mb_pdu_tx_size;

void mock_mb_pdu_tx (
   mb_transport_t * transport,
   const pdu_txn_t * transaction,
   size_t size);

extern unsigned int mock_mb_pdu_rx_calls;
extern const uint8_t * mock_mb_pdu_rx_data;
extern uint8_t mock_mb_pdu_rx_size;
extern int mock_mb_pdu_rx_result;

int mock_mb_pdu_rx (
   mb_transport_t * transport,
   pdu_txn_t * transaction,
   uint32_t tmo);

bool mock_mb_pdu_rx_bc (mb_transport_t * transport);
bool mock_mb_pdu_rx_avail (mb_transport_t * transport);

#ifdef __cplusplus
}
#endif

#endif /* MOCKS_H */
