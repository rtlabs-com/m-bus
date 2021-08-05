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

/* Do not include winsock.h */
#define WIN32_LEAN_AND_MEAN
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include "mbal_tcp.h"
#include "mb_tcp.h"
#include "osal.h"

#include <winsock2.h>
#include <Mstcpip.h>
#include <stdio.h>

#define KEEP_ALIVE_IDLE  10 /* max idle time before keepalive sent [s] */
#define KEEP_ALIVE_INTVL 2  /* time between keepalives [s] */
#define KEEP_ALIVE_CNT   3  /* max number of unacked keepalives */

/* max time to wait for message in progress [ms] */
#define RCV_TIMEOUT 500

static void os_winsock_init (void)
{
   static int winsock_init = 0;

   if (!winsock_init)
   {
      /* Initialise winsock */
      WSADATA wsaData;
      WSAStartup (MAKEWORD (2, 2), &wsaData);
      winsock_init = 1;
   }
}

int os_tcp_connect (const char * name, uint16_t port)
{
   int result;
   SOCKET sock;
   int option;
   struct sockaddr_in addr;

   os_winsock_init();

   /* Create socket */
   sock = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP);
   if (sock == INVALID_SOCKET)
   {
      return -1;
   }

   /* Set socket options (nagle, reuseaddr) */

   option = 1;
   result =
      setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error;
   }

   option = 1;
   result =
      setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error;
   }

   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = inet_addr (name);
   addr.sin_port = htons (port);

   /* Connect to peer */
   result = connect (sock, (struct sockaddr *)&addr, sizeof (addr));
   if (result == SOCKET_ERROR)
   {
      goto error;
   }

   return (int)sock;

error:
   closesocket (sock);
   return -1;
}

int os_tcp_accept_connection (uint16_t port)
{
   int result;
   SOCKET sock;
   struct sockaddr_in addr;
   DWORD tv;
   int option;
   SOCKET peer;

   os_winsock_init();

   /* Create listening socket */
   sock = socket (AF_INET, SOCK_STREAM, 0);
   if (sock == INVALID_SOCKET)
   {
      return -1;
   }

   memset (&addr, 0, sizeof (addr));

   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl (INADDR_ANY);
   addr.sin_port = htons (port);

   option = 1; /* enable */
   result =
      setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error1;
   }

   tv = RCV_TIMEOUT;
   result =
      setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof (tv));
   if (result == SOCKET_ERROR)
   {
      goto error1;
   }

   result = bind (sock, (struct sockaddr *)&addr, sizeof (addr));
   if (result == SOCKET_ERROR)
   {
      goto error1;
   }

   result = listen (sock, 1);
   if (result == SOCKET_ERROR)
   {
      goto error1;
   }

   /* Accept one connection */
   peer = accept (sock, NULL, NULL);
   if (peer == INVALID_SOCKET)
   {
      /* Error or timeout */
      goto error1;
   }

   /* Set peer socket options (nagle, reuseaddr, receive timeout,
      keepalive). */

   option = 1;
   result =
      setsockopt (peer, IPPROTO_TCP, TCP_NODELAY, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error2;
   }

   option = 1; /* enable */
   result =
      setsockopt (peer, SOL_SOCKET, SO_REUSEADDR, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error2;
   }

   tv = RCV_TIMEOUT;
   result =
      setsockopt (peer, SOL_SOCKET, SO_RCVTIMEO, (char *)&tv, sizeof (tv));
   if (result == SOCKET_ERROR)
   {
      goto error2;
   }

   option = 1;
   result =
      setsockopt (peer, SOL_SOCKET, SO_KEEPALIVE, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error2;
   }

   /* keepalive is not available on windows */
#if 0
   option = KEEP_ALIVE_IDLE;
   result = setsockopt (peer, IPPROTO_TCP, TCP_KEEPIDLE, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error2;
   }

   option = KEEP_ALIVE_INTVL;
   result =
      setsockopt (peer, IPPROTO_TCP, TCP_KEEPINTVL, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error2;
   }

   option = KEEP_ALIVE_CNT;
   result = setsockopt (peer, IPPROTO_TCP, TCP_KEEPCNT, (char *)&option, sizeof (int));
   if (result == SOCKET_ERROR)
   {
      goto error2;
   }
#endif

   /* Close listening socket. No more connections accepted. */
   closesocket (sock);
   return (int)peer;

error2:
   closesocket (peer);
error1:
   closesocket (sock);
   return -1;
}

void os_tcp_close (int peer)
{
   SOCKET s = (SOCKET)peer;
   closesocket (s);
}

int os_tcp_send (int peer, const void * buffer, size_t size)
{
   SOCKET s = (SOCKET)peer;
   const uint8_t * p = buffer;
   int remain = (int)size;
   int n;

   do
   {
      n = send (s, p, remain, 0);
      remain -= n;
      p += n;
   } while (remain > 0 && n > 0);

   if (remain == 0)
   {
      /* Successfully sent all data */
      return (int)size;
   }

   /* Connection closed or error sending */
   return n;
}

int os_tcp_recv (int peer, void * buffer, size_t size)
{
   SOCKET s = (SOCKET)peer;
   uint8_t * p = buffer;
   int remain = (int)size;
   int n;

   do
   {
      n = recv (s, p, remain, 0);
      remain -= n;
      p += n;
   } while (remain > 0 && n > 0);

   if (remain == 0)
   {
      /* Successfully received all data */
      return (int)size;
   }

   /* Connection closed or error receiving */
   return n;
}

int os_tcp_recv_wait (int peer, uint32_t tmo)
{
   SOCKET s = (SOCKET)peer;
   fd_set fds;
   int result;
   struct timeval tv;

   FD_ZERO (&fds);
   FD_SET (s, &fds);

   tv.tv_sec = 0;
   tv.tv_usec = tmo * 1000;
   result = select (0, &fds, NULL, NULL, &tv);
   return result;
}
