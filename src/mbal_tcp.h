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

#ifndef MBAL_TCP_H
#define MBAL_TCP_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mbal_sys.h"
#include "mb_transport.h"

int os_tcp_connect (const char * name, uint16_t port);
int os_tcp_accept_connection (uint16_t port);
void os_tcp_close (int peer);
int os_tcp_send (int peer, const void * buffer, size_t size);
int os_tcp_recv (int peer, void * buffer, size_t size);
int os_tcp_recv_wait (int peer, uint32_t tmo);

#ifdef __cplusplus
}
#endif

#endif /* MBAL_TCP_H */
