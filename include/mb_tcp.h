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
 * \file
 * Modbus TCP transport data layer
 */

#ifndef MB_TCP_H
#define MB_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_export.h"

#include <stdint.h>

/**
 * Default TCP server port
 */
#define MODBUS_DEFAULT_PORT 502

/**
 * TCP layer configuration
 */
typedef struct mb_tcp_cfg
{
   uint16_t port; /**< TCP server port.
                   *   Modbus slave will listen on this local port.
                   *   Modbus master will connect to slaves at this remote port.
                   */

   /**
    * This callback function is called when a new connection has been
    * accepted.
    *
    * The callback can be disabled if not required, by setting it to
    * NULL.
    *
    * \param transport          handle
    */
   void (*up_cb) (mb_transport_t * transport);

   /**
    * This callback function is called when a connection is shut down.
    *
    * The callback can be disabled if not required, by setting it to
    * NULL.
    *
    * \param transport          handle
    */
   void (*down_cb) (mb_transport_t * transport);
} mb_tcp_cfg_t;

/**
 * Initialise and configure the Modbus TCP data layer
 *
 * This function allocates memory needed by the instance and initialises it.
 * A CC_ASSERT() is triggered upon memory allocation failure.
 * User may use the returned handle when initialising a Modbus master or slave
 * instance.
 *
 * The following example creates a Modbus TCP layer instance with the default
 * TCP server port for Modbus:
 * \code
 * mb_transport_t * transport;
 * static const mb_tcp_cfg_t tcp_cfg =
 * {
 *    .port = MODBUS_DEFAULT_PORT,
 * };
 * transport = mb_tcp_init (&tcp_cfg);
 * \endcode
 *
 * \see mbus_create() and mb_slave_init().
 *
 * \param cfg           TCP layer configuration
 *
 * \return Handle to be used in further operations
 */
MB_EXPORT mb_transport_t * mb_tcp_init (const mb_tcp_cfg_t * cfg);

#ifdef __cplusplus
}
#endif

#endif /* MB_TCP_H */

