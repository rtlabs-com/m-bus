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

/*
 * Modbus master
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

/**
 * Modbus master
 *
 * Instantiated at startup using mbus_create() or mbus_init().
 *
 * The contents of this structure should be interpreted as an implementation
 * detail. Direct access by user application is prohibited.
 */
typedef struct mbus
{
   uint32_t timeout;                /**< \private */
   mb_transport_t * transport;      /**< \private */
   pdu_txn_t transaction;           /**< \private */
   void * scratch;                  /**< \private */
} mbus_t;

/**
 * Modbus table selection
 *
 * These are the primary tables in the Modbus data model.
 */
typedef enum mb_table
{
   MB_TABLE_COILS = 0,              /**< Coils. Single bit read/write */
   MB_TABLE_INPUTS = 1,             /**< Inputs. Single bit read-only */
   MB_TABLE_INPUT_REGISTERS = 3,    /**< Input registers. 16 bits read-only */
   MB_TABLE_HOLDING_REGISTERS = 4,  /**< Holding registers. 16 bits read/write */
} mb_table_t;

/**
 * Modbus address
 *
 * May be constructed using MB_ADDRESS().
 */
typedef uint32_t mb_address_t;

/**
 * Return a Modbus address using the given \a table and \a
 * address. The allowed Modbus tables are:
 *
 *  <TABLE>
 *  <TR><TD>\b 0</TD>        <TD>Coils</TD></TR>
 *  <TR><TD>\b 1</TD>        <TD>Inputs</TD></TR>
 *  <TR><TD>\b 3</TD>        <TD>Input registers</TD></TR>
 *  <TR><TD>\b 4</TD>        <TD>Holding registers</TD></TR>
 *  </TABLE>
 *
 * The enumeration mb_table_t may also be used.
 * The following two calls are equivalent:
 * \code
 * mb_address_t address1 = MB_ADDRESS (3, 30001);
 * mb_address_t address2 = MB_ADDRESS (MB_TABLE_INPUT_REGISTERS, 30001);
 * \endcode
 *
 * \param table         Table. Valid values: see above or \a mb_table_t
 * \param address       Address. Valid range: 1 - 65536
 * \return              Modbus address
 */
#define MB_ADDRESS(table, address) ((table) << 16 | ((address) & 0xFFFF))

/**
 * Master configuration
 */
typedef struct mbus_cfg
{
   uint32_t timeout;    /**< Receive-timeout in milliseconds.
                         *   A slave failing to respond in time will cause
                         *   master to abort its request and return an ETIMEOUT
                         *   error to user.
                         */
} mbus_cfg_t;

/**
 * Create an instance of the Modbus master stack
 *
 * This function allocates memory needed by the instance, followed by
 * a call to mbus_init(), which initialises it.
 * A CC_ASSERT() is triggered upon memory allocation failure.
 * User may use the returned handle in further operations.
 *
 * In the following example, an instance is created with TCP as the
 * transport data layer:
 * \code
 * mb_transport_t * transport;
 * mbus_t * mbus;
 * static const mb_tcp_cfg_t transport_cfg =
 * {
 *    .port = 502,
 * };
 * static const mbus_cfg_t master_cfg =
 * {
 *    .timeout = 1000,
 * };
 *
 * transport = mb_tcp_init (&transport_cfg);
 * mbus = mbus_create (&master_cfg, transport);
 * \endcode
 *
 * \param cfg           Modbus configuration
 * \param transport     Modbus transport data layer.
 *                      Must be initialised. See mb_rtu_init() and
 *                      mb_tcp_init().
 *                      The stack assumes ownership of this object.
 *
 * \return Modbus handle to be used in further operations
 */
MB_EXPORT mbus_t * mbus_create (
   const mbus_cfg_t * cfg,
   mb_transport_t * transport);

/**
 * Initialise an instance of the Modbus master stack
 *
 * This is an alternative to mbus_create() when use of dynamic memory allocation
 * is not desired.
 *
 * \param mbus          Modbus handle
 * \param cfg           Modbus configuration
 * \param transport     Modbus transport data layer.
 *                      Must be initialised. See mb_rtu_init() and
 *                      mb_tcp_init().
 *                      The stack assumes ownership of this object.
 * \param scratch       Scratch data array (MAX_PDU_SIZE bytes).
 *                      The stack assumes ownership of this object.
 */
MB_EXPORT void mbus_init (
   mbus_t * mbus,
   const mbus_cfg_t * cfg,
   mb_transport_t * transport,
   uint8_t * scratch);

/**
 * Return a handle to the transport data layer.
 *
 * \param mbus         Modbus handle
 *
 * \return Handle to transport data layer
 */
MB_EXPORT void * mbus_transport_get (mbus_t * mbus);

/**
 * Connect to a Modbus slave
 *
 * The following example connects to a slave over TCP/IP:
 * \code
 * int slave = mbus_connect (mbus, "192.168.10.134");
 * \endcode
 *
 * \param mbus         Modbus handle
 * \param name         Slave identifier
 *
 * \return Slave handle on success, error code otherwise
 */
MB_EXPORT int mbus_connect (mbus_t * mbus, const char * name);

/**
 * Disconnect from a Modbus slave
 *
 * \param mbus         Modbus handle
 * \param slave        Slave handle
 *
 * \return 0 always
 */
MB_EXPORT int mbus_disconnect (mbus_t * mbus, int slave);

/**
 * Read Modbus addresses
 *
 * This function reads a number of Modbus address contents starting
 * from the given address. The function will read coils, inputs,
 * holding registers or input registers as specified by the given
 * starting address.
 *
 * Note that addresses are 1-based. 0 is thus an invalid address.
 *
 * For holding or input register reads, the returned buffer contents
 * will be in the correct byte format. Register contents are
 * byte-swapped from network byte order, if required.
 * The following example would read 10 input registers from address 30001 to
 * 30010:
 * \code
 * mb_address_t address = MB_ADDRESS (MB_TABLE_INPUT_REGISTERS, 30001);
 * uint16_t buffer[10];
 * result = mbus_read (mbus, slave, address, 10, buffer);
 * \endcode
 *
 * For coil and input status reads, the returned buffer contents will
 * be a bit string where the LSB is the bit that was read from the
 * starting address, and the MSB is the bit that was read from the
 * final address.
 * The following example would read 10 inputs from address 10001 to 10010.
 * Note that 10 bits fit in two bytes:
 * \code
 * mb_address_t address = MB_ADDRESS (MB_TABLE_INPUTS, 10001);
 * uint8_t buffer[2];
 * result = mbus_read (mbus, slave, address, 10, buffer);
 * \endcode
 *
 * \param mbus          Modbus handle
 * \param slave         Slave handle. See mbus_connect()
 * \param address       1-based starting address. May be constructed
 *                      by calling MB_ADDRESS()
 * \param quantity      Number of addresses to read. Valid range is 1 - 2000
 *                      for coils/inputs and 1 - 125 for registers
 * \param buffer        Output buffer. Must be large enough to hold the
 *                      requested data
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
 * Write Modbus addresses
 *
 * This function writes a number of Modbus addresses starting from the
 * given address. The function will write coils or holding registers
 * as specified by the given starting address.
 *
 * Note that addresses are 1-based. 0 is thus an invalid address.
 *
 * For holding register writes, the \a buffer shall be a number of
 * 16-bit register values in the CPU byte ordering. Register values
 * are byte-swapped to network byte ordering before transmission, if
 * required.
 * The following example would write 3 holding registers from address 40001
 * to 40003:
 * \code
 * mb_address_t address = MB_ADDRESS (MB_TABLE_HOLDING_REGISTERS, 40001);
 * uint16_t buffer[3] = {0x1234, 0x5678, 0x90ab};
 * result = mbus_write (mbus, slave, address, 3, buffer);
 * \endcode
 *
 * For coil writes, the \a buffer shall be a bit string where the
 * LSB is the bit that will be written to the starting address, and
 * the MSB is the bit that will be written to the final address.
 * The following example would write 100 coils from address 23 to 122.
 * Coils 23 to 30 are set to 1 while coils 31 to 122 are set to 0:
 * \code
 * mb_address_t address = MB_ADDRESS (MB_TABLE_COILS, 23);
 * uint8_t buffer[13] = {0xff,0,0,0,0,0,0,0,0,0,0,0,0};
 * result = mbus_write (mbus, slave, address, 100, buffer);
 * \endcode
 *
 * \param mbus          Modbus handle
 * \param slave         Slave handle. See mbus_connect()
 * \param address       1-based starting address. May be constructed
 *                      by calling MB_ADDRESS(). Note that only coils and
 *                      holding registers are writeable.
 * \param quantity      Number of addresses to write. Valid range is 1 - 1968
 *                      for coils and 1 - 123 for holding registers
 * \param buffer        Input buffer
 *
 * \return 0 on success, error code otherwise
 */
MB_EXPORT int mbus_write (
   mbus_t * mbus,
   int slave,
   mb_address_t address,
   uint16_t quantity,
   const void * buffer);

/**
 * Write a single Modbus address
 *
 * This function writes a single Modbus address. The function will
 * write a single coil or holding register as specified by the given
 * address.
 *
 * Note that addresses are 1-based. 0 is thus an invalid address.
 *
 * For holding register writes, the value shall be in the CPU byte
 * ordering. The value is byte-swapped to network byte ordering before
 * transmission, if required
 * The following example would set the holding register at address 40001
 * to the value 0x1234:
 * \code
 * mb_address_t address = MB_ADDRESS (MB_TABLE_HOLDING_REGISTERS, 40001);
 * result = mbus_write_single (mbus, slave, address, 0x1234);
 * \endcode
 *
 * For coil writes, the coil shall be set if value is non-zero, or
 * cleared if the value is zero.
 * The following example would set the coil at address 23:
 * \code
 * mb_address_t address = MB_ADDRESS (MB_TABLE_COILS, 23);
 * result = mbus_write_single (mbus, slave, address, 1);
 * \endcode
 *
 * \param mbus          Modbus handle
 * \param slave         Slave handle. See mbus_connect()
 * \param address       1-based address to be written to. May be constructed
 *                      by calling MB_ADDRESS(). Note that only coils and
 *                      holding registers are writeable.
 * \param value         The value to be written
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
 * This function issues the diagnostic loopback function. The \a buffer
 * contents are sent to the slave. The data read from the slave is placed
 * into the \a buffer (up to a maximum of \a size).
 *
 * The following example sends four bytes to \a slave and then receives up to
 * four bytes back:
 * \code
 * uint8_t buffer[4]  = {0x11, 0x22, 0x33, 0x44};
 * size = mbus_loopback (mbus, slave, sizeof (buffer), buffer);
 * \endcode
 *
 * \param mbus          Modbus handle
 * \param slave         Slave handle. See mbus_connect()
 * \param size          The number of bytes of data to send
 * \param buffer        Input/output buffer
 *
 * \return Number of bytes received on success, error code otherwise
 */
MB_EXPORT int mbus_loopback (
   mbus_t * mbus,
   int slave,
   uint16_t size,
   void * buffer);

/**
 * Send raw message
 *
 * This function sends the message in \a msg to the Modbus slave. The
 * message contents are not interpreted by the stack.
 *
 * The following example would send one byte to the \a slave:
 * \code
 * uint8_t msg[1] = {101};
 * result = mbus_send_msg (mbus, slave, msg, sizeof (msg));
 * \endcode
 *
 * \param mbus          Modbus handle
 * \param slave         Slave handle. See mbus_connect()
 * \param msg           Message to send
 * \param size          Size of message
 *
 * \return 0 on success, error code otherwise
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
 * message contents are not interpreted by the stack.
 *
 * The caller must provide a \a buffer large enough to hold the requested
 * data.
 *
 * The following example would receive up to 253 bytes from \a slave:
 * \code
 * int size;
 * uint8_t msg[253];
 * size = mbus_get_msg (mbus, slave, msg, sizeof (msg));
 * \endcode
 *
 * \param mbus          Modbus handle
 * \param slave         Slave handle. See mbus_connect()
 * \param msg           Message received
 * \param size          Max size of message. If message size is not known in
 *                      advance, use a buffer of size 253 bytes in order to
 *                      prevent buffer overflow.
 *
 * \return Number of bytes received on success, error code otherwise
 */
MB_EXPORT int mbus_get_msg (mbus_t * mbus, int slave, void * msg, uint16_t size);

#ifdef __cplusplus
}
#endif

#endif /* MBUS_H */

