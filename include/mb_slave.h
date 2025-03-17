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
 * \file
 * Modbus slave
 */

#ifndef MB_SLAVE_H
#define MB_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_error.h"

#include "mb_export.h"

/**
 * I/O table interface
 *
 * This is the interface the slave task uses when reading and writing
 * to I/Os belonging to a table. The table could be coils, inputs, holding
 * registers and input registers.
 */
typedef struct mb_iotable
{
   /**
    * Number of valid addresses in table.
    */
   size_t size;

   /**
    * Getter callback
    *
    * This function is called in response to a read
    * operation. The function should perform the get operation by
    * storing the requested data in the \a data array.
    *
    * The mb_slave_bit_set() and mb_slave_reg_set() functions can be
    * used to set the contents of the data array.
    *
    * The address supplied to the input parameter is 0-based.
    *
    * \param address            Starting address (0-based)
    * \param data               Data array to store the requested data
    * \param quantity           Number of addresses to get
    *
    * \return 0 on success, ESLAVE_DEVICE_FAILURE exception otherwise
    */
   int (*get) (uint16_t address, uint8_t * data, size_t quantity);

   /**
    * Setter callback
    *
    * This function is called in response to a write
    * operation. The function should perform the set operation using
    * the data provided in the \a data array.
    *
    * The mb_slave_bit_get() and mb_slave_reg_get() functions can be
    * used to extract the contents of the data array.
    *
    * This callback is not used for input status and input register
    * tables, and should then be set to NULL.
    *
    * The address supplied to the input parameter is 0-based.
    *
    * \param address            Starting address (0-based)
    * \param data               Data array with the supplied data
    * \param quantity           Number of addresses to set
    *
    * \return 0 on success, ESLAVE_DEVICE_FAILURE exception otherwise
    */
   int (*set) (uint16_t address, uint8_t * data, size_t quantity);

} mb_iotable_t;

/**
 * Vendor-defined function
 */
typedef struct mb_vendor_func
{
   /**
    * Vendor-defined function code
    */
   uint8_t function;

   /**
    * Function callback
    *
    * This function is called in response to
    * receiving the function code specified in \a function.
    *
    * The \a data array holds the received request data. The number of
    * valid bytes in \a data is given by \a size. The first byte of the
    * data is the function code.
    *
    * The callback may return a response by storing it in the \a data
    * array. The callback should return the number of valid bytes in
    * the response, or 0 if no response is to be sent. The response
    * should normally include the function code unchanged.
    *
    * The callback can return a Modbus exception if an error
    * occurs. See mb_error.h for available exceptions.
    *
    * An extended exception can be sent by manually setting the MSB of
    * the function code (\a data[0]), and following it with the
    * exception data, returning the total number of valid bytes.
    *
    * \param data               Request data, including the function code
    * \param size               Size of request data (bytes)
    *
    * \return Size of response on success, Modbus exception otherwise
    */
   int (*callback) (uint8_t * data, size_t size);

} mb_vendor_func_t;

/**
 * I/O map
 */
typedef struct mb_iomap
{
   mb_iotable_t coils;             /**< Coil definitions */
   mb_iotable_t inputs;            /**< Input status definitions */
   mb_iotable_t holding_registers; /**< Holding register definitions */
   mb_iotable_t input_registers;   /**< Input register definitions */
   size_t num_vendor_funcs;        /**< Number of vendor-defined functions */
   const mb_vendor_func_t * vendor_funcs; /**< Vendor-defined functions */
} mb_iomap_t;

/**
 * Slave configuration
 */
typedef struct mb_slave_cfg
{
   uint8_t id;                      /**< Slave ID */
   uint32_t priority;               /**< Priority of slave task */
   size_t stack_size;               /**< Stack size of slave task (in bytes) */
   const mb_iomap_t * iomap;        /**< Slave I/O map */
} mb_slave_cfg_t;

/**
 * Modbus slave
 *
 * Instantiated by calling mb_slave_init().
 *
 * The contents of this structure should be interpreted as an implementation
 * detail. Direct access by user application is prohibited.
 */
typedef struct mb_slave
{
   uint8_t id;                      /**< \private */
   int running;                     /**< \private */
   mb_transport_t * transport;      /**< \private */
   const mb_iomap_t * iomap;        /**< \private */
} mb_slave_t;

/**
 * Create an instance of the Modbus slave stack
 *
 * This function allocates memory needed by the instance, initialises it
 * and starts a task for serving requests from the Modbus master.
 * A CC_ASSERT() is triggered upon memory allocation failure.
 * User may use the returned handle in further operations.
 *
 * The \a cfg parameter is used to configure the slave behaviour. It
 * contains the I/O map which defines how the slave responds to Modbus
 * requests.
 *
 * The following examples illustrates how to write a simple
 * slave:
 *
 * \code
 * #include <mb_slave.h>
 * #include <stdbool.h>
 *
 * static bool coils[16] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
 * static uint16_t hold[4] = { 0x0000, 0x0000, 0x0000, 0x0000 };
 *
 * static int coil_get (uint16_t address, uint8_t * data, size_t quantity)
 * {
 *    uint16_t offset;
 *
 *    for (offset = 0; offset < quantity; offset++)
 *    {
 *       uint32_t bit = address + offset;
 *
 *       mb_slave_bit_set (data, offset, coils[bit]);
 *    }
 *    return 0;
 * }
 *
 * static int coil_set (uint16_t address, uint8_t * data, size_t quantity)
 * {
 *    uint16_t offset;
 *
 *    for (offset = 0; offset < quantity; offset++)
 *    {
 *       uint32_t bit = address + offset;
 *
 *       coils[bit] = mb_slave_bit_get (data, offset);
 *    }
 *    return 0;
 * }
 *
 * static int input_get (uint16_t address, uint8_t * data, size_t quantity)
 * {
 *    uint16_t offset;
 *
 *    for (offset = 0; offset < quantity; offset++)
 *    {
 *       mb_slave_bit_set (data, offset, 0);
 *    }
 *    return 0;
 * }
 *
 * static int hold_get (uint16_t address, uint8_t * data, size_t quantity)
 * {
 *    uint16_t offset;
 *
 *    for (offset = 0; offset < quantity; offset++)
 *    {
 *       uint32_t reg = address + offset;
 *
 *       mb_slave_reg_set (data, offset, hold[reg]);
 *    }
 *    return 0;
 * }
 *
 * static int hold_set (uint16_t address, uint8_t * data, size_t quantity)
 * {
 *    uint16_t offset;
 *
 *    for (offset = 0; offset < quantity; offset++)
 *    {
 *       uint32_t reg = address + offset;
 *
 *       hold[reg] = mb_slave_reg_get (data, offset);
 *    }
 *    return 0;
 * }
 *
 * static int reg_get (uint16_t address, uint8_t * data, size_t quantity)
 * {
 *    uint16_t offset;
 *
 *    for (offset = 0; offset < quantity; offset++)
 *    {
 *       mb_slave_reg_set (data, offset, 0x1234);
 *    }
 *    return 0;
 * }
 *
 * static int ping (uint8_t * data, size_t rx_count)
 * {
 *    char * message = "Hello World";
 *    memcpy (data, message, strlen (message));
 *    return strlen (message);
 * }
 *
 * static const mb_vendor_func_t vendor_funcs[] =
 * {
 *    { 101, ping },
 * };
 *
 * static const mb_iomap_t mb_slave_iomap =
 * {
 *    .coils             = { 16, coil_get, coil_set },  // 16 coils
 *    .inputs            = { 2, input_get, NULL },      // 2 input status bits
 *    .holding_registers = { 4, hold_get, hold_set },   // 4 holding registers
 *    .input_registers   = { 5, reg_get, NULL },        // 5 input registers
 *    .num_vendor_funcs  = NELEMENTS (vendor_funcs),    // 1 vendor function
 *    .vendor_funcs      = vendor_funcs,
 * };
 *
 * static const mb_rtu_cfg_t mb_rtu_cfg =
 * {
 *    .serial = "/sio0",
 *    .sio_cfg   = &sio_cfg,
 *    .tx_enable = tx_en,
 *    .tmr_init  = mb_tmr_init,
 *    .tmr_start = mb_tmr_start,
 * };
 *
 * static const mb_slave_cfg_t mb_slave_cfg =
 * {
 *    .id = 2,                          // Slave ID: 2
 *    .priority = 15,
 *    .stack_size = 1024,
 *    .iomap = &mb_slave_iomap
 * };
 *
 * mb_slave_t * start_slave (void)
 * {
 *    mb_slave_t * slave;
 *    mb_transport_t * rtu;
 *
 *    rtu = mb_rtu_init (&mb_rtu_cfg);
 *    slave = mb_slave_init (&mb_slave_cfg, rtu);
 *    return slave;
 * }
 * \endcode
 *
 * The example would create an RTU Modbus slave with ID 2. The slave has 16
 * coils, 2 input status bits, 4 holding registers and 5 input
 * registers. The input status bits return the constant value 0,
 * whereas the input registers return the constant value 0x1234. The
 * coils and holding registers can be written and read. There is one
 * vendor function (function code 101) that returns the string "Hello World"
 * when called.
 *
 * The callbacks should return 0 on success, or a Modbus exception code as
 * documented in mb_error.h, except for the vendor function callback
 * which returns the size of the response or a Modbus exception code.
 *
 * This function returns a handle to the slave which can be used for
 * further operations as documented below.
 *
 * \param cfg           Slave configuration
 * \param transport     Handle to transport data layer.
 *                      Must be initialised. See mb_rtu_init() and
 *                      mb_tcp_init().
 *                      The stack assumes ownership of this object.
 *
 * \return Slave handle to be used in further operations
 */
MB_EXPORT mb_slave_t * mb_slave_init (
   const mb_slave_cfg_t * cfg,
   mb_transport_t * transport);

/**
 * Shut down a running slave
 *
 * This function orders the slave task to shut down.
 * The slave task will eventually exit after serving any pending requests.
 *
 * \param slave         Slave handle
 */
MB_EXPORT void mb_slave_shutdown (mb_slave_t * slave);

/**
 * Return a handle to the transport data layer
 *
 * \param slave         Slave handle
 *
 * \return Handle to the transport data layer
 */
MB_EXPORT void * mb_slave_transport_get (mb_slave_t * slave);

/**
 * Change the slave ID
 *
 * \param slave         Slave handle
 * \param id            New ID
 */
MB_EXPORT void mb_slave_id_set (mb_slave_t * slave, uint8_t id);

/**
 * Get bit value from Modbus data
 *
 * This function gets the bit with index \a address in the
 * \a data array. The function is intended for use by the coils- and
 * inputs callbacks.
 *
 * In the following example, the value 1 is retrieved from the bit string
 * \a data at \a address 28:
 *
 * \code
 * uint8_t data[4] = {0x00, 0x00, 0x00, 0x10};
 * int value = mb_slave_bit_get (data, 28);
 * \endcode
 *
 * \param data          Bit-string (Modbus encoded)
 * \param address       0-based index of bit to get
 *
 * \return 0 if bit is clear, 1 if bit is set
 */
MB_EXPORT int mb_slave_bit_get (const void * data, uint32_t address);

/**
 * Set bit value in Modbus data
 *
 * This function sets the bit with index \a address in the \a data array
 * to \a value. The function is intended for use by the coils- and inputs
 * callbacks.
 *
 * In the following example, bit 4 in byte 3 is set to 1:
 *
 * \code
 * uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};
 * mb_slave_bit_set (data, 28, 1);
 * \endcode
 *
 * \param data          Bit-string (Modbus encoded)
 * \param address       0-based index of bit to set
 * \param value         new value (0 to clear, non-zero to set)
 */
MB_EXPORT void mb_slave_bit_set (void * data, uint32_t address, int value);

/**
 * Get register value from Modbus data
 *
 * This function gets the register with index \a address in the \a
 * data array. The data will be converted from network byte ordering
 * if required. The function is intended for use by the
 * holding_registers.set and input_registers.set callbacks.
 *
 * In the following example, the value 0x1234 is retrieved from the Modbus
 * \a data at \a address 1:
 *
 * \code
 * uint8_t data[4] = {0x00, 0x00, 0x12, 0x34};
 * uint16_t value = mb_slave_reg_get (data, 1);
 * \endcode
 *
 * \param data          Register array (Modbus encoded)
 * \param address       0-based index of register to get
 *
 * \return register value
 */
MB_EXPORT uint16_t mb_slave_reg_get (const void * data, uint32_t address);

/**
 * Set register value in Modbus data
 *
 * This function sets the register with index \a address in the \a
 * data array. The data is converted to network byte ordering if
 * required. The function is intended for use by the
 * holding_registers.get and input_registers.get callbacks.
 *
 * In the following example, byte 2 in the buffer \a data is set to 0x12
 * while byte 3 is set to 0x34.
 *
 * \code
 * uint8_t data[4] = {0x00, 0x00, 0x00, 0x00};
 * mb_slave_reg_set (data, 1, 0x1234);
 * \endcode
 *
 * \param data          Register array (Modbus encoded)
 * \param address       0-based index of register to set
 * \param value         New value
 */
MB_EXPORT void mb_slave_reg_set (void * data, uint32_t address, uint16_t value);

/**
 * \private
 */
void mb_slave_handle_request (mb_slave_t * slave, pdu_txn_t * transaction);

#ifdef __cplusplus
}
#endif

#endif /* MB_SLAVE_H */
