/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * http://www.rt-labs.com
 * Copyright 2011 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#ifdef __rtk__

/**
 * \addtogroup mb_master Modbus master
 * \{
 */

#ifndef MB_MASTER_H
#define MB_MASTER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mbus.h"

#include <drivers/dev.h>
#include <unistd.h>
#include <fcntl.h>
#include <shell.h>

/* Modbus shell commands */
extern const shell_cmd_t cmd_mb_read;
extern const shell_cmd_t cmd_mb_write;

typedef mbus_cfg_t mb_master_cfg_t;

/**
 * Initialise the modbus master stack
 */
drv_t * mb_master_init (
   const char * name,
   const mb_master_cfg_t * cfg,
   mb_transport_t * transport);

/**
 * Open modbus slave communication stream
 *
 * This function opens a communication stream to a modbus device. The
 * modbus device is specified through the filename, ie:
 *
 * \code
 * /modbus0/15
 * \endcode
 *
 * indicates the RTU slave with ID decimal 15 on modbus bus 0, whereas:
 *
 * \code
 * /modbus1/192.168.10.101
 * \endcode
 *
 * indicates the TCP slave with IP 192.168.10.101 on modbus bus 1.
 *
 * \param name          slave identifier
 *
 * \return handle to be used in all further modbus operations
 */
static inline int mb_open (const char * name)
{
   return open (name, O_RDWR, 0);
}

/**
 * Close modbus slave communication stream
 *
 * This function closes a communication stream to a modbus device.
 *
 * \param fd            modbus handle
 */
static inline void mb_close (int fd)
{
   close (fd);
}

/**
 * Return a handle to the transport data layer.
 *
 * \param fd           modbus handle
 *
 * \return handle
 */
void * mb_master_transport_get (int fd);

/**
 * Read modbus addresses
 *
 * This function reads a number of modbus address contents starting
 * from the given address. The function will read coils, inputs,
 * holding registers or input registers as specified by the given
 * starting address.
 *
 * The address is 1-based and can be specified by the MB_ADDRESS
 * macro, like so:
 *
 * \code
 * result = mb_read (fd, MB_ADDRESS (3, 10001), 10, buffer);
 * \endcode
 *
 * The example would read 10 input registers starting from modbus
 * address 3x10001.
 *
 * The caller must provide a buffer large enough to hold the requested
 * data.
 *
 * For coil and input status reads, the returned buffer contents will
 * be a bit string where the LSB is the bit that was read from the
 * starting address, and the MSB is the bit that was read from the
 * final address.
 *
 * For holding or input register reads, the returned buffer contents
 * will be in the correct byte format. Register contents are
 * byte-swapped from network byte order, if required.
 *
 * \param fd            modbus handle
 * \param address       1-based starting address
 * \param quantity      number of addresses to read
 * \param buffer        output buffer
 *
 * \return 0 on success, error code otherwise
 */
int mb_read (int fd, mb_address_t address, uint16_t quantity, void * buffer);

/**
 * Write modbus addresses
 *
 * This function writes a number of modbus addresses starting from the
 * given address. The function will write coils or holding registers
 * as specified by the given starting address.
 *
 * The address is 1-based and can be specified by the MB_ADDRESS macro, like so:
 *
 * \code
 * result = mb_write (fd, MB_ADDRESS (0, 23), 100, buffer);
 * \endcode
 *
 * The example would write 100 coils starting from modbus address
 * 0x23.
 *
 * For coil writes, the input buffer shall be a bit string where the
 * LSB is the bit that will be written to the starting address, and
 * the MSB is the bit that will be written to the final address.
 *
 * For holding register writes, the input buffer shall be a number of
 * 16-byte register values in the CPU byte ordering. Register values
 * are byte-swapped to network byte ordering before transmission, if
 * required.
 *
 * \param fd            modbus handle
 * \param address       1-based starting address
 * \param quantity      number of addresses to read
 * \param buffer        input buffer
 *
 * \return 0 on success, error code otherwise
 */
int mb_write (int fd, mb_address_t address, uint16_t quantity, void * buffer);

/**
 * Write a single modbus address
 *
 * This function writes a single modbus address. The function will
 * write a single coil or holding register as specified by the given
 * address.
 *
 * The is 1-based address and can be specified by the MB_ADDRESS macro, like so:
 *
 * \code
 * result = mb_write_single (fd, MB_ADDRESS (0, 23), 1);
 * \endcode
 *
 * The example would set the coil at modbus address 0x23.
 *
 * For coil writes, the coil shall be set if value is non-zero, or
 * cleared if the value is zero.
 *
 * For holding register writes, the value shall be in the CPU byte
 * ordering. The value is byte-swapped to network byte ordering before
 * transmission, if required.
 *
 * \param fd            modbus handle
 * \param address       1-based address to be written to
 * \param value         the value to be written
 *
 * \return 0 on success, error code otherwise
 */
int mb_write_single (int fd, mb_address_t address, uint16_t value);

/**
 * Diagnostic loopback
 *
 * This function issues the diagnostic loopback function. The buffer
 * contents are sent the slave. The data read from the slave is placed
 * into the buffer (up to a maximum of \a size).
 *
 * \param fd            modbus handle
 * \param size          the number of bytes of data to send
 * \param buffer        the value to be written
 *
 * \return number of bytes received on success, error code otherwise
 */
int mb_loopback (int fd, uint16_t size, void * buffer);

/**
 * Send raw message
 *
 * This function sends the message in \a msg to the modbus slave. The
 * message contents are ignored.
 *
 * \param fd            modbus handle
 * \param msg           message to send
 * \param size          size of message
 *
 * \return error code
 */
int mb_send_msg (int fd, const void * msg, uint8_t size);

/**
 * Get raw message
 *
 * This function returns the next message received from the slave. The
 * message contents are ignored.
 *
 * The caller must provide a buffer large enough to hold the requested
 * data.
 *
 * \param fd            modbus handle
 * \param msg           message received
 * \param size          max size of message
 *
 * \return number of bytes received on success, error code otherwise
 */
int mb_get_msg (int fd, void * msg, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* MB_MASTER_H */

/**
 * \}
 */

#endif /* __rtk__ */
