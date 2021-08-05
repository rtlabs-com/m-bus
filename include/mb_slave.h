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
 * \addtogroup mb_slave Modbus slave
 * \{
 */

#ifndef MB_SLAVE_H
#define MB_SLAVE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_error.h"

#include "mb_export.h"

typedef struct mb_iotable
{
   /**
    * Number of valid addresses in table.
    */
   size_t size;

   /**
    * Get callback. This function is called in response to a read
    * operation. The function should perform the get operation by
    * storing the requested data in the \a data array.
    *
    * The mb_slave_bit_set() and mb_slave_reg_set() functions can be
    * used to set the contents of the data array.
    *
    * This callback is not used for input status and input register
    * tables, and should then be set to NULL.
    *
    * The address supplied to the input parameter is 0-based.
    *
    * The callback can return a modbus exception if an error
    * occurs. See mb_error.h for available exceptions.
    *
    * \param address            Starting address
    * \param data               Data array
    * \param quantity           Number of addresses to set
    *
    * \return 0 on success, modbus exception otherwise
    */
   int (*get) (uint16_t address, uint8_t * data, size_t quantity);

   /**
    * Set callback. This function is called in response to a write
    * operation. The function should perform the set operation using
    * the data provided in the \a data array.
    *
    * The mb_slave_bit_get() and mb_slave_reg_get() functions can be
    * used to extract the contents of the data array.
    *
    * The addresses supplied to the input parameter is 0-based.
    *
    * The callback can return a modbus exception if an error
    * occurs. See mb_error.h for available exceptions.
    *
    * \param address            Starting address
    * \param data               Data array
    * \param quantity           Number of addresses to set
    *
    * \return 0 on success, modbus exception otherwise
    */
   int (*set) (uint16_t address, uint8_t * data, size_t quantity);

} mb_iotable_t;

typedef struct mb_vendor_func
{
   /**
    * Vendor-defined function code.
    */
   uint8_t function;

   /**
    * Function callback. This function is called in response to
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
    * The callback can return a modbus exception if an error
    * occurs. See mb_error.h for available exceptions.
    *
    * An extended exception can be sent by manually setting the MSB of
    * the function code (\a data[0]), and following it with the
    * exception data, returning the total number of valid bytes.
    *
    * \param data               Request data, incl. function code
    * \param size               Size of request data (bytes)
    *
    * \return Size of response on success, modbus exception otherwise
    */
   int (*callback) (uint8_t * data, size_t size);

} mb_vendor_func_t;

typedef struct mb_iomap
{
   mb_iotable_t coils;             /**< Coil definitions */
   mb_iotable_t inputs;            /**< Input status definitions */
   mb_iotable_t holding_registers; /**< Holding register definitions */
   mb_iotable_t input_registers;   /**< Input register definitions */
   size_t num_vendor_funcs;        /**< Number of vendor-defined functions */
   const mb_vendor_func_t * vendor_funcs; /**< Vendor-defined functions */
} mb_iomap_t;

typedef struct mb_slave_cfg
{
   uint8_t id;               /**< Slave ID */
   uint32_t priority;        /**< Priority of slave task */
   size_t stack_size;        /**< Stack size of slave task*/
   const mb_iomap_t * iomap; /**< Slave iomap */
} mb_slave_cfg_t;

typedef struct mb_slave
{
   uint8_t id;
   int running;
   mb_transport_t * transport;
   const mb_iomap_t * iomap;
} mb_slave_t;

/**
 * Initialise the modbus slave stack
 *
 * The \a cfg parameter is used to configure the slave behaviour. It
 * contains the iomap which defines how the slave responds to modbus
 * requests. The following examples illustrates how to write a simple
 * slave.
 *
 * \code
 * #include <modbus/mb_slave.h>
 *
 * static uint8_t coils[2] = { 0 };
 * static uint16_t hold[4] = { 0 };
 *
 * static int coil_get (uint16_t address, uint8_t * data, size_t quantity)
 * {
 *    uint16_t offset;
 *
 *    for (offset = 0; offset < quantity; offset++)
 *    {
 *       uint32_t bit = address + offset;
 *       int value;
 *
 *       value = mb_slave_bit_get (coils, bit);
 *       mb_slave_bit_set (data, offset, value);
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
 *       int value;
 *
 *       value = mb_slave_bit_get (data, offset);
 *       mb_slave_bit_set (coils, bit, value);
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
 *    memcpy (data, message, strlen(message));
 *    return strlen(message);
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
 * mb_slave_t * mb_start (void)
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
 * The example would create an RTU modbus slave with ID 2. The slave has 16
 * coils, 2 input status bits, 4 holding registers and 5 input
 * registers. The input status bits return the constant value 0,
 * whereas the input registers return the constant value 0x1234. The
 * coils and holding registers can be written and read. There is one
 * vendor function (function code 101) that returns the string "Hello
 * World" when called.
 *
 * The callbacks return 0 on success, or a modbus exception code as
 * documented in mb_error.h, except for the vendor function callback
 * which returns the size of the response or a modbus exception code.
 *
 * This function returns a handle to the slave which can be used for
 * further operations as documented below.
 *
 * \param cfg           Slave configuration
 * \param transport     Handle to transport data layer
 *
 * \return slave handle to be used in further operations
 */
MB_EXPORT mb_slave_t * mb_slave_init (
   const mb_slave_cfg_t * cfg,
   mb_transport_t * transport);

/**
 * Shutdown a running slave.
 *
 * The slave will exit after serving pending requests.
 *
 * \param slave         slave handle
 */
MB_EXPORT void mb_slave_shutdown (mb_slave_t * slave);

/**
 * Return a handle to the transport data layer.
 *
 * \param slave         slave handle
 *
 * \return handle
 */
MB_EXPORT void * mb_slave_transport_get (mb_slave_t * slave);

/**
 * Change the slave ID.
 *
 * \param slave         handle
 * \param id            new ID
 */
MB_EXPORT void mb_slave_id_set (mb_slave_t * slave, uint8_t id);

/**
 * Get a bit in the bit-string \a data
 *
 * This function gets the bit with index \a address in the bit-string
 * \a data. The function is intended for use by the coils- and
 * inputs callbacks.
 *
 * \param data          bit-string
 * \param address       0-based index of bit to set
 *
 * \return 0 if bit is clear, 1 if bit is set
 */
MB_EXPORT int mb_slave_bit_get (void * data, uint32_t address);

/**
 * Set a bit in the bit-string \a data
 *
 * This function sets the bit with index \a address in the bit-string
 * \a data. The function is intended for use by the coils- and inputs
 * callbacks.
 *
 * \param data          bit-string
 * \param address       0-based index of bit to set
 * \param value         new value (0 to clear, non-zero to set)
 */
MB_EXPORT void mb_slave_bit_set (void * data, uint32_t address, int value);

/**
 * Get register value from modbus data.
 *
 * This function gets the register with index \a address in the \a
 * data array. The data will be converted from network byte ordering
 * if required. The function is intended for use by the
 * holding_registers.set and input_registers.set callbacks.
 *
 * \param data          register array
 * \param address       0-based index of register to get
 *
 * \return register value
 */
MB_EXPORT uint16_t mb_slave_reg_get (void * data, uint32_t address);

/**
 * Set register value in modbus data.
 *
 * This function sets the register with index \a address in the \a
 * data array. The data is converted to network byte ordering if
 * required. The function is intended for use by the
 * holding_registers.get and input_registers.get callbacks.
 *
 * \param data          register array
 * \param address       0-based index of register to set
 * \param value         new value
 */
MB_EXPORT void mb_slave_reg_set (void * data, uint32_t address, uint16_t value);

/**
 * \internal
 */
void mb_slave_handle_request (mb_slave_t * slave, pdu_txn_t * transaction);

#ifdef __cplusplus
}
#endif

#endif /* MB_SLAVE_H */

/**
 * \}
 */
