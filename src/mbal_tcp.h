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
 * Modbus TCP platform abstraction layer
 */

#ifndef MBAL_TCP_H
#define MBAL_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mbal_sys.h"
#include "mb_transport.h"

/**
 * Connect to remote server
 *
 * Creates a socket and attempts to establish a TCP connection with remote
 * server.
 *
 * Only used by Modbus master
 *
 * \param name       Remote server IP address, as string
 * \param port       Remote server port
 * \return           Socket descriptor for TCP connection if successful,
 *                   negative number otherwise
 */
int os_tcp_connect (const char * name, uint16_t port);

/**
 * Wait for connection from remote client
 *
 * Listens for incoming TCP connections. Accepts the first one and returns
 * socket descriptor for that connection.
 *
 * Only used by Modbus slave
 *
 * \param port       Local server port
 * \return           Socket descriptor for TCP connection if successful,
 *                   negative number otherwise
 */
int os_tcp_accept_connection (uint16_t port);

/**
 * Close TCP connection
 *
 * \param peer       Socket descriptor for TCP connection
 */
void os_tcp_close (int peer);

/**
 * Send data to connected TCP peer
 *
 * An attempt will be made to send all of the data in \a buffer.
 *
 * \param peer       Socket descriptor for TCP connection
 * \param buffer     Data to send
 * \param size       Size of \a buffer in bytes
 * \returns          Same as \a size if successful,
 *                   0 or negative number otherwise
 */
int os_tcp_send (int peer, const void * buffer, size_t size);

/**
 * Receive data from connected TCP peer
 *
 * Waits for all requested data to be received and then stores it in \a buffer.
 *
 * \param peer       Socket descriptor for TCP connection
 * \param buffer     Buffer to store received data
 * \param size       Size of \a buffer in bytes
 * \returns          Same as \a size if successful,
 *                   0 or negative number otherwise
 */
int os_tcp_recv (int peer, void * buffer, size_t size);

/**
 * Wait for received data from connected TCP peer
 *
 * Waits for any data to be available for reading without reading it.
 *
 * \param peer       Socket descriptor for TCP connection
 * \param tmo        Timeout in milliseconds
 * \returns          Positive number if successful,
 *                   0 or negative number otherwise
 */
int os_tcp_recv_wait (int peer, uint32_t tmo);

#ifdef __cplusplus
}
#endif

#endif /* MBAL_TCP_H */
