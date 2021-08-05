/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2012 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "mbal_rtu.h"
#include "options.h"

#include "osal.h"
#include "osal_log.h"

#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/epoll.h>
#include <termios.h>
#include <unistd.h>
#include <string.h>

static void * mb_arg;
static int mb_fd;

ssize_t os_rtu_write (int fd, const void * buffer, size_t size)
{
   ssize_t nwrite;
   nwrite = write (fd, buffer, size);
   return nwrite;
}

ssize_t os_rtu_read (int fd, void * buffer, size_t size)
{
   ssize_t nread;
   nread = read (fd, buffer, size);
   return nread;
}

void os_rtu_tx_drain (int fd, size_t size)
{
   float char_time_us;

   char_time_us = 11.0 * 1000 * 1000 / 115200;
   os_usleep ((uint32_t)char_time_us);
}

ssize_t os_rtu_rx_avail (int fd)
{
   int navail = 0;
   if (ioctl (fd, FIONREAD, &navail) < 0)
   {
      return -1;
   }
   return navail;
}

void os_rtu_set_serial_cfg (int fd, const mb_rtu_serial_cfg_t * cfg)
{
   struct termios tio;

   memset (&tio, 0, sizeof (tio));

   /* Setup raw processing */
   tio.c_cflag |= CS8 | CLOCAL | CREAD;

   switch (cfg->baudrate)
   {
   case 1200:
      tio.c_cflag |= B1200;
      break;
   case 1800:
      tio.c_cflag |= B1800;
      break;
   case 2400:
      tio.c_cflag |= B2400;
      break;
   case 4800:
      tio.c_cflag |= B4800;
      break;
   case 9600:
      tio.c_cflag |= B9600;
      break;
   case 19200:
      tio.c_cflag |= B19200;
      break;
   case 38400:
      tio.c_cflag |= B38400;
      break;
   case 57600:
      tio.c_cflag |= B57600;
      break;
   case 115200:
      tio.c_cflag |= B115200;
      break;
   case 230400:
      tio.c_cflag |= B230400;
      break;
   case 460800:
      tio.c_cflag |= B460800;
      break;
   default:
      tio.c_cflag |= B19200;
      break;
   }

   switch (cfg->parity)
   {
   case ODD:
      tio.c_cflag |= PARENB | PARODD;
      tio.c_iflag |= INPCK | ISTRIP;
      break;
   case EVEN:
      tio.c_cflag |= PARENB;
      tio.c_iflag |= INPCK | ISTRIP;
      break;
   case NONE:
      break;
   default:
      break;
   }

   tio.c_cc[VMIN] = 1;
   tio.c_cc[VTIME] = 0;

   tcflush (fd, TCIFLUSH);
   tcsetattr (fd, TCSANOW, &tio);
}

static void os_rtu_rx (void * arg)
{
   struct epoll_event ev, events[1];
   int epollfd;
   int nfds;
   int n;

   epollfd = epoll_create1 (0);
   if (epollfd == -1)
   {
      LOG_ERROR (MB_RTU_LOG, "epoll_create1 failed\n");
      return;
   }

   /* Create edge-triggered event on input */
   ev.events = EPOLLIN | EPOLLET;
   ev.data.fd = mb_fd;
   if (epoll_ctl (epollfd, EPOLL_CTL_ADD, mb_fd, &ev) == -1)
   {
      LOG_ERROR (MB_RTU_LOG, "epoll_ctl failed\n");
      return;
   }

   for (;;)
   {
      nfds = epoll_wait (epollfd, events, 1, -1);
      if (nfds == -1)
      {
         if (errno == EINTR)
            continue;

         LOG_ERROR (MB_RTU_LOG, "epoll_wait failed\n");
         return;
      }

      for (n = 0; n < nfds; n++)
      {
         if (events[n].data.fd == mb_fd)
         {
            mb_rx_hook (mb_arg, NULL);
         }
      }
   }
}

int os_rtu_open (const char * name, void * arg)
{
   int fd;

   mb_arg = arg;

   fd = open (name, O_RDWR | O_NOCTTY | O_NONBLOCK, 0);
   assert (fd != -1);
   mb_fd = fd;

   os_thread_create ("mb_rtu_rx", 5, 1024, os_rtu_rx, arg);
   return fd;
}
