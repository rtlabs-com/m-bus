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
 * \addtogroup mb_tcp Modbus TCP data layer
 * \{
 */

#ifndef MB_TCP_H
#define MB_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_export.h"

#include <stdint.h>

#define MODBUS_DEFAULT_PORT 502

typedef struct mb_tcp_cfg
{
   uint16_t port;
} mb_tcp_cfg_t;

typedef struct mb_tcp mb_tcp_t;

/**
 * Initialise and configure the Modbus TCP data layer.
 *
 * \param cfg           TCP layer configuration
 *
 * \return handle to be used in further operations
 */
MB_EXPORT mb_transport_t * mb_tcp_init (const mb_tcp_cfg_t * cfg);

#endif /* MB_TCP_H */

/**
 * \}
 */
