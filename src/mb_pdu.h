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

#ifndef PDU_H
#define PDU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "osal.h"

#define MAX_PDU_SIZE 253

/* PDU function codes */
#define PDU_READ_COILS                   1
#define PDU_READ_INPUTS                  2
#define PDU_READ_HOLDING_REGISTERS       3
#define PDU_READ_INPUT_REGISTERS         4
#define PDU_WRITE_COIL                   5
#define PDU_WRITE_HOLDING_REGISTER       6
#define PDU_DIAGNOSTICS                  8
#define PDU_WRITE_COILS                  15
#define PDU_WRITE_HOLDING_REGISTERS      16
#define PDU_READ_WRITE_HOLDING_REGISTERS 23

/* Diagnostic sub-functions */
#define PDU_DIAG_LOOPBACK 0

typedef struct pdu_exception
{
   uint8_t function;
   uint8_t code;
} pdu_exception_t;

typedef struct pdu_request
{
   uint8_t function;
} pdu_request_t;

typedef pdu_request_t pdu_response_t;

CC_PACKED_BEGIN
typedef struct pdu_read
{
   uint8_t function;
   uint16_t address;
   uint16_t quantity;
} CC_PACKED pdu_read_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct pdu_read_response
{
   uint8_t function;
   uint8_t count;
   uint8_t data[];
} CC_PACKED pdu_read_response_t;
CC_PACKED_END

CC_STATIC_ASSERT (sizeof (pdu_read_response_t) == 2);

CC_PACKED_BEGIN
typedef struct pdu_write_single
{
   uint8_t function;
   uint16_t address;
   uint16_t value;
} CC_PACKED pdu_write_single_t;
CC_PACKED_END

CC_STATIC_ASSERT (sizeof (pdu_write_single_t) == 5);

typedef pdu_write_single_t pdu_write_single_response_t;

CC_PACKED_BEGIN
typedef struct pdu_write
{
   uint8_t function;
   uint16_t address;
   uint16_t quantity;
   uint8_t count;
   uint8_t data[];
} CC_PACKED pdu_write_t;
CC_PACKED_END

CC_STATIC_ASSERT (sizeof (pdu_write_t) == 6);

CC_PACKED_BEGIN
typedef struct pdu_write_response
{
   uint8_t function;
   uint16_t address;
   uint16_t quantity;
} CC_PACKED pdu_write_response_t;
CC_PACKED_END

CC_STATIC_ASSERT (sizeof (pdu_write_response_t) == 5);

CC_PACKED_BEGIN
typedef struct pdu_read_write
{
   uint8_t function;
   uint16_t read_address;
   uint16_t read_quantity;
   uint16_t write_address;
   uint16_t write_quantity;
   uint8_t count;
   uint8_t data[];
} CC_PACKED pdu_read_write_t;
CC_PACKED_END

CC_STATIC_ASSERT (sizeof (pdu_read_write_t) == 10);

CC_PACKED_BEGIN
typedef struct pdu_diag_t
{
   uint8_t function;
   uint16_t sub_function;
   uint8_t data[];
} CC_PACKED pdu_diag_t;
CC_PACKED_END

CC_PACKED_BEGIN
typedef struct pdu_vendor_t
{
   uint8_t function;
   uint8_t data[];
} CC_PACKED pdu_vendor_t;
CC_PACKED_END

typedef union
{
   pdu_exception_t exception;
   pdu_request_t request;
   pdu_response_t response;
   pdu_read_t read;
   pdu_read_response_t read_response;
   pdu_write_single_t write_single;
   pdu_write_single_response_t write_single_response;
   pdu_write_t write;
   pdu_write_response_t write_response;
   pdu_read_write_t read_write;
   pdu_diag_t diag;
   pdu_vendor_t vendor;
} pdu_t;

#ifdef __cplusplus
}
#endif

#endif /* PDU_H */

/**
 * \}
 */
