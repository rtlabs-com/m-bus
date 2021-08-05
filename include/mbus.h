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

/**
 * \addtogroup mbus Modbus master
 * \{
 */

#ifndef MBUS_H
#define MBUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_error.h"

#include "mb_export.h"

#include <stdint.h>

typedef struct mbus
{
   uint32_t timeout;
   mb_transport_t * transport;
   pdu_txn_t transaction;
   void * scratch;
} mbus_t;

typedef uint32_t mb_address_t;

/**
 * Return a modbus address using the given \a table and \a
 * address. The allowed modbus tables are:
 *
 *  <TABLE>
 *  <TR><TD>\b 0</TD>        <TD>Coils</TD></TR>
 *  <TR><TD>\b 1</TD>        <TD>Inputs</TD></TR>
 *  <TR><TD>\b 3</TD>        <TD>Input registers</TD></TR>
 *  <TR><TD>\b 4</TD>        <TD>Holding registers</TD></TR>
 *  </TABLE>
 */
#define MB_ADDRESS(table, address) ((table) << 16 | ((address)&0xFFFF))

typedef struct mbus_cfg
{
   uint32_t timeout;
} mbus_cfg_t;

/**
 * Create an instance of the modbus master stack
 *
 * \param cfg           modbus configuration
 * \param transport     modbus transport data layer
 *
 * \return modbus handle
 */
MB_EXPORT mbus_t * mbus_create (
   const mbus_cfg_t * cfg,
   mb_transport_t * transport);

/**
 * Initialise an instance of the modbus master stack
 *
 * \param mbus          modbus handle
 * \param cfg           modbus configuration
 * \param transport     modbus transport data layer
 * \param scratch       scratch data array (MAX_PDU_SIZE bytes)
 */
MB_EXPORT void mbus_init (
   mbus_t * mbus,
   const mbus_cfg_t * cfg,
   mb_transport_t * transport,
   uint8_t * scratch);

/**
 * Return a handle to the transport data layer.
 *
 * \param mbus         modbus handle
 *
 * \return handle
 */
MB_EXPORT void * mbus_transport_get (mbus_t * mbus);

/**
 * Connect to a modbus slave
 *
 * \param mbus         modbus handle
 * \param name         Slave identifier
 *
 * \return slave handle on success, error code otherwise
 */
MB_EXPORT int mbus_connect (mbus_t * mbus, const char * name);

/**
 * Disconnect from a modbus slave
 *
 * \param mbus         modbus handle
 * \param slave        slave handle
 *
 * \return 0 always
 */
MB_EXPORT int mbus_disconnect (mbus_t * mbus, int slave);

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
 * result = mbus_read (fd, MB_ADDRESS (3, 10001), 10, buffer);
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
 * \param mbus          modbus handle
 * \param address       1-based starting address
 * \param quantity      number of addresses to read
 * \param buffer        output buffer
 *
 * \return 0 on success, error code otherwise
 */
MB_EXPORT int mbus_read (
   mbus_t * mbus,
   int slave,
   mb_address_t address,
   uint16_t quantity,
   void * buffer);

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
 * result = mbus_write (fd, MB_ADDRESS (0, 23), 100, buffer);
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
 * \param mbus          modbus handle
 * \param address       1-based starting address
 * \param quantity      number of addresses to read
 * \param buffer        input buffer
 *
 * \return 0 on success, error code otherwise
 */
MB_EXPORT int mbus_write (
   mbus_t * mbus,
   int slave,
   mb_address_t address,
   uint16_t quantity,
   void * buffer);

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
 * result = mbus_write_single (fd, MB_ADDRESS (0, 23), 1);
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
 * \param mbus          modbus handle
 * \param address       1-based address to be written to
 * \param value         the value to be written
 *
 * \return 0 on success, error code otherwise
 */
MB_EXPORT int mbus_write_single (
   mbus_t * mbus,
   int slave,
   mb_address_t address,
   uint16_t value);

/**
 * Diagnostic loopback
 *
 * This function issues the diagnostic loopback function. The buffer
 * contents are sent the slave. The data read from the slave is placed
 * into the buffer (up to a maximum of \a size).
 *
 * \param mbus          modbus handle
 * \param size          the number of bytes of data to send
 * \param buffer        the value to be written
 *
 * \return number of bytes received on success, error code otherwise
 */
MB_EXPORT int mbus_loopback (
   mbus_t * mbus,
   int slave,
   uint16_t size,
   void * buffer);

/**
 * Send raw message
 *
 * This function sends the message in \a msg to the modbus slave. The
 * message contents are ignored.
 *
 * \param mbus          modbus handle
 * \param msg           message to send
 * \param size          size of message
 *
 * \return error code
 */
MB_EXPORT int mbus_send_msg (
   mbus_t * mbus,
   int slave,
   const void * msg,
   uint8_t size);

/**
 * Get raw message
 *
 * This function returns the next message received from the slave. The
 * message contents are ignored.
 *
 * The caller must provide a buffer large enough to hold the requested
 * data.
 *
 * \param mbus          modbus handle
 * \param msg           message received
 * \param size          max size of message
 *
 * \return number of bytes received on success, error code otherwise
 */
MB_EXPORT int mbus_get_msg (mbus_t * mbus, int slave, void * msg, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* MBUS_H */

/**
 * \}
 */
