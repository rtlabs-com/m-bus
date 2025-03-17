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

/**
 * Modbus RTU platform abstraction layer
 */

#ifndef MBAL_RTU_H
#define MBAL_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mbal_sys.h"
#include "mb_rtu.h"

/**
 * Inform M-Bus stack that a byte is available in Rx FIFO
 *
 * Implemented by the M-Bus stack. May be called by port-specific implementation
 * in mbal_rtu.c when at least one received byte is available.
 *
 * This function will also restart the t1p5 and t3p5 timers.
 *
 * \param arg        Pointer to RTU instance, as returned from mb_rtu_init()
 * \param data       Unused parameter. May be NULL
 * \return           Always 0
 */
int mb_rx_hook (void * arg, void * data);

/**
 * Inform M-Bus stack that no more bytes are available in Tx FIFO
 *
 * Implemented by the M-Bus stack. May be called by port-specific implementation
 * in mbal_rtu.c when Tx FIFO is empty.
 *
 * \param arg        Pointer to RTU instance, as returned from mb_rtu_init()
 * \param data       Unused parameter. May be NULL
 * \return           Always 0
 */
int mb_tx_hook (void * arg, void * data);

/**
 * Write up to \a size bytes to Tx FIFO
 *
 * \param fd         File descriptor for serial port
 * \param buffer     Buffer with data to be written
 * \param size       Size of buffer in bytes
 * \return           Number of bytes written if successful,
 *                   0 or negative number on failure
 *
 */
ssize_t os_rtu_write (int fd, const void * buffer, size_t size);

/**
 * Read up to \a size bytes from Rx FIFO into \a buffer
 *
 * \param fd         File descriptor for serial port
 * \param buffer     Buffer to store data
 * \param size       Size of buffer in bytes. May be 0
 * \return           Number of bytes read into buffer if successful,
 *                   0 or negative number on failure
 */
ssize_t os_rtu_read (int fd, void * buffer, size_t size);

/**
 * Drain Tx FIFO
 *
 * \param fd         File descriptor for serial port
 * \param size       Number of bytes that have been written to Tx FIFO
 */
void os_rtu_tx_drain (int fd, size_t size);

/**
 * Get number of bytes in Rx FIFO
 *
 * \param fd         File descriptor for serial port
 * \return           Number of bytes available for reading (0 or more),
 *                   negative number on failure
 */
ssize_t os_rtu_rx_avail (int fd);

/**
 * Configure serial port
 *
 * \param fd         File descriptor for serial port
 * \param cfg        Configuration (baudrate, parity)
 */
void os_rtu_set_serial_cfg (int fd, const mb_rtu_serial_cfg_t * cfg);

/**
 * Open serial port
 *
 * \param name       Name of serial port
 * \param arg        Pointer to RTU instance, as returned from mb_rtu_init()
 * \return           File descriptor for serial port on success,
 *                   negative number on failure
 */
int os_rtu_open (const char * name, void * arg);

#ifdef __cplusplus
}
#endif

#endif /* MBAL_RTU_H */
