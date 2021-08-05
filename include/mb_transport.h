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

/**
 * \addtogroup mb_transport Modbus transport layer
 * \{
 */

#ifndef MB_TRANSPORT_H
#define MB_TRANSPORT_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_error.h"
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>

typedef struct pdu_txn
{
   int arg;       /**< Transport peer identifier */
   uint16_t id;   /**< Transaction ID */
   uint8_t unit;  /**< Transaction Unit ID */
   uint8_t flags; /**< Transaction flags */
   void * data;   /**< Data for transaction */
} pdu_txn_t;

typedef struct mb_transport mb_transport_t;
struct mb_transport
{
   int (*bringup) (mb_transport_t * transport, const char * name);
   int (*shutdown) (mb_transport_t * transport, int arg);
   bool (*is_down) (mb_transport_t * transport);
   void (*tx) (
      mb_transport_t * transport,
      const pdu_txn_t * transaction,
      size_t size);
   int (*rx) (mb_transport_t * transport, pdu_txn_t * transaction, uint32_t tmo);
   bool (*rx_is_bc) (mb_transport_t * transport);
   bool (*rx_avail) (mb_transport_t * transport);
   bool is_server;
};

int mb_transport_bringup (mb_transport_t * transport, const char * name);

int mb_transport_shutdown (mb_transport_t * transport, int arg);

bool mb_transport_is_down (mb_transport_t * transport);

/**
 * Transmit modbus protocol data unit (PDU). The PDU will be formatted
 * according to the application data unit protocol (ADU) of the
 * underlying transport protocol.
 *
 * \param transport     handle
 * \param slave         Slave modbus ID
 * \param pdu           PDU to send
 * \param size          Size of PDU
 */
void mb_pdu_tx (
   mb_transport_t * transport,
   const pdu_txn_t * transaction,
   size_t size);

/**
 * Receive modbus protocol data unit (PDU). The PDU is extracted from
 * the application data unit protocol (ADU) of the underlying
 * transport protocol.
 *
 * The PDU must be of size MAX_PDU_SIZE, in order to hold the maximum
 * message size.
 *
 * The function will return with ETIMEOUT if no message was received
 * in \a tmo ticks. A \a tmo of 0 means to wait forever. See mb_error.h
 * for other possible error codes.
 *
 * \param transport     handle
 * \param slave         Slave modbus ID
 * \param pdu           Received PDU
 * \param tmo           Timeout
 *
 * \return size of pdu on success, negative error code otherwise
 */
int mb_pdu_rx (
   mb_transport_t * transport,
   pdu_txn_t * transaction,
   uint32_t tmo);

/**
 * Return true if last received PDU was part of a broadcast message,
 * false otherwise
 *
 * \param transport     handle
 *
 * \return true if last received PDU was part of a broadcast message,
 * false otherwise
 */
bool mb_pdu_rx_bc (mb_transport_t * transport);

/**
 * Return true if data has been received, false otherwise
 *
 * \param transport     handle
 *
 * \return true if data has been received, false otherwise
 */
bool mb_pdu_rx_avail (mb_transport_t * transport);

#ifdef __cplusplus
}
#endif

#endif /* MB_TRANSPORT_H */

/**
 * \}
 */
