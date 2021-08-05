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

#include "mbal_tcp.h"
#include "mb_tcp.h"

#include <unistd.h>
#include <sys/socket.h>
#include <string.h>

#if 0
#define PERROR(s) perror ("modbus: "s)
#else
#define PERROR(s)
#endif

#define KEEP_ALIVE_IDLE  10 /* max idle time before keepalive sent [s] */
#define KEEP_ALIVE_INTVL 2  /* time between keepalives [s] */
#define KEEP_ALIVE_CNT   3  /* max number of unacked keepalives */

/* max time to wait for message in progress [ms] */
#define RCV_TIMEOUT 500

int os_tcp_connect (const char * name, uint16_t port)
{
   int result;
   int sock;
   int option;
   struct sockaddr_in addr;

   /* Create socket */
   sock = socket (AF_INET, SOCK_STREAM, 0);
   if (sock == -1)
   {
      PERROR ("socket");
      return -1;
   }

   /* Set socket options (nagle, reuseaddr) */

   option = 1;
   result = setsockopt (sock, IPPROTO_TCP, TCP_NODELAY, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("TCP_NODELAY");
      goto error;
   }

   option = 1;
   result = setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("SO_REUSEADDR");
      goto error;
   }

   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = inet_addr (name);
   addr.sin_port = htons (port);

   /* Connect to peer */
   result = connect (sock, (struct sockaddr *)&addr, sizeof (addr));
   if (result == -1)
   {
      goto error;
   }

   return sock;

error:
   close (sock);
   return -1;
}

int os_tcp_accept_connection (uint16_t port)
{
   int result;
   int sock;
   struct sockaddr_in addr;
   int option;
   int peer;

   /* Create listening socket */
   sock = socket (AF_INET, SOCK_STREAM, 0);
   if (sock == -1)
   {
      PERROR ("socket");
      return -1;
   }

   memset (&addr, 0, sizeof (addr));

   addr.sin_family = AF_INET;
   addr.sin_addr.s_addr = htonl (INADDR_ANY);
   addr.sin_port = htons (port);

   option = 1; /* enable */
   result = setsockopt (sock, SOL_SOCKET, SO_REUSEADDR, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("SO_REUSEADDR");
      goto error1;
   }

   option = RCV_TIMEOUT;
   result = setsockopt (sock, SOL_SOCKET, SO_RCVTIMEO, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("SO_RCVTIMEO");
      goto error1;
   }

   result = bind (sock, (struct sockaddr *)&addr, sizeof (addr));
   if (result == -1)
   {
      PERROR ("bind");
      goto error1;
   }

   result = listen (sock, 1);
   if (result == -1)
   {
      PERROR ("listen");
      goto error1;
   }

   /* Accept one connection */
   peer = accept (sock, NULL, NULL);
   if (peer == -1)
   {
      /* Error or timeout */
      goto error1;
   }

   /* Set peer socket options (nagle, reuseaddr, receive timeout,
      keepalive). */

   option = 1;
   result = setsockopt (peer, IPPROTO_TCP, TCP_NODELAY, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("TCP_NODELAY");
      goto error2;
   }

   option = 1; /* enable */
   result = setsockopt (peer, SOL_SOCKET, SO_REUSEADDR, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("SO_REUSEADDR");
      goto error2;
   }

   option = RCV_TIMEOUT;
   result = setsockopt (peer, SOL_SOCKET, SO_RCVTIMEO, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("SO_RCVTIMEO");
      goto error2;
   }

   option = 1;
   result = setsockopt (peer, SOL_SOCKET, SO_KEEPALIVE, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("SO_KEEPALIVE");
      goto error2;
   }

   option = KEEP_ALIVE_IDLE;
   result = setsockopt (peer, IPPROTO_TCP, TCP_KEEPIDLE, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("TCP_KEEPALIVE");
      goto error2;
   }

   option = KEEP_ALIVE_INTVL;
   result =
      setsockopt (peer, IPPROTO_TCP, TCP_KEEPINTVL, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("TCP_KEEPINTVL");
      goto error2;
   }

   option = KEEP_ALIVE_CNT;
   result = setsockopt (peer, IPPROTO_TCP, TCP_KEEPCNT, &option, sizeof (int));
   if (result == -1)
   {
      PERROR ("TCP_KEEPCNT");
      goto error2;
   }

   /* Close listening socket. No more connections accepted. */
   close (sock);
   return peer;

error2:
   close (peer);
error1:
   close (sock);
   return -1;
}

void os_tcp_close (int peer)
{
   close (peer);
}

int os_tcp_send (int peer, const void * buffer, size_t size)
{
   const uint8_t * p = buffer;
   size_t remain = size;
   int n;

   do
   {
      n = send (peer, p, remain, 0);
      remain -= n;
      p += n;
   } while (remain > 0 && n > 0);

   if (remain == 0)
   {
      /* Successfully sent all data */
      return size;
   }

   /* Connection closed or error sending */
   return n;
}

int os_tcp_recv (int peer, void * buffer, size_t size)
{
   uint8_t * p = buffer;
   size_t remain = size;
   int n;

   do
   {
      n = recv (peer, p, remain, 0);
      remain -= n;
      p += n;
   } while (remain > 0 && n > 0);

   if (remain == 0)
   {
      /* Successfully received all data */
      return size;
   }

   /* Connection closed or error receiving */
   return n;
}

int os_tcp_recv_wait (int peer, uint32_t tmo)
{
   fd_set fds;
   int result;
   struct timeval tv;

   FD_ZERO (&fds);
   FD_SET (peer, &fds);

   tv.tv_sec = 0;
   tv.tv_usec = tmo * 1000;
   result = select (peer + 1, &fds, NULL, NULL, &tv);
   return result;
}
