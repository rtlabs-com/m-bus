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
 * Modbus RTU transport data layer
 */

#ifndef MB_RTU_H
#define MB_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_export.h"

#include <stdint.h>

/**
 * Serial port parity setting
 */
typedef enum mb_rtu_parity
{
   ODD,                    /**< Odd parity */
   EVEN,                   /**< Even parity */
   NONE,                   /**< No parity */
} mb_rtu_parity_t;

/**
 * Serial port configuration
 */
typedef struct mb_rtu_serial_cfg
{
   int baudrate;           /**< Baud rate [bits/s] */
   mb_rtu_parity_t parity; /**< Parity bit setting */
} mb_rtu_serial_cfg_t;

/**
 * RTU layer configuration
 */
typedef struct mb_rtu_cfg
{
   uint8_t id; /**< \private Unused field kept for compatibility */

   /**
    * Name of serial port to use, e.g. "/sio0"
    */
   const char * serial;

   /**
    * Serial port configuration
    */
   const mb_rtu_serial_cfg_t * serial_cfg;

   /**
    * This callback function is called before and after
    * transmission. It should enable or disable the serial port for
    * transmission, as indicated by \a level.
    *
    * The callback can be disabled if not required, by setting it to
    * NULL.
    *
    * \param level              1 to enable transmission, 0 to disable
    */
   void (*tx_enable) (int level);

   /**
    * This function should initialise the T1P5 and T3P5 one-shot timers, using
    * the given timeouts.
    *
    * Note that it may be possible to use a single timer if it has at
    * least two match values.
    *
    * \param t1p5               T1P5 timeout [us]. This is the maximum tolerated
    *                           time between transferred bytes. Exceeding this
    *                           limit may be interpreted as an end-of-frame
    *                           condition. Its value is calculated by the stack
    *                           based on the baud rate.
    *                           A typical value is 750 us.
    * \param t3p5               T3P5 timeout [us]. This is the minimum tolerated
    *                           time between transferred frames. Two frames
    *                           separated by a smaller delay may be interpreted
    *                           as a single frame. Its value is calculated by
    *                           the stack. A typical value is 1750 us.
    */
   void (*tmr_init) (uint32_t t1p5, uint32_t t3p5);

   /**
    * This function should start or restart the T1P5 and T3P5 one-shot timers.
    *
    * \param t1p5_expired       Function to be called when T1P5 expires.
    *                           If NULL, timer should not be started.
    * \param t3p5_expired       Function to be called when T3P5 expires.
    *                           If NULL, timer should not be started.
    * \param arg                Argument passed to t1p5_expired and t3p5_expired
    */
   void (*tmr_start) (
      void (*t1p5_expired) (void * arg),
      void (*t3p5_expired) (void * arg),
      void * arg);
} mb_rtu_cfg_t;

/**
 * Reconfigure the Modbus RTU serial parameters
 *
 * Calling this function reconfigures the serial port and calculates
 * the T1P5 and T3P5 timeouts.
 *
 * Example:
 * \code
 * static const mb_rtu_serial_cfg_t serial_cfg =
 * {
 *    .baudrate = 115200,
 *    .parity = NONE,
 * };
 * mb_rtu_serial_cfg (rtu, &serial_cfg);
 * \endcode
 *
 * \param rtu           RTU layer handle
 * \param serial_cfg    Serial port configuration
 */
MB_EXPORT void mb_rtu_serial_cfg (
   mb_transport_t * rtu,
   const mb_rtu_serial_cfg_t * serial_cfg);

/**
 * Initialise and configure the Modbus RTU data layer
 *
 * This function allocates memory needed by the instance and initialises it.
 * A CC_ASSERT() is triggered upon memory allocation failure.
 * The serial port is also opened and configured.
 * User may use the returned handle when initialising a Modbus master or slave
 * instance.
 *
 * The following example creates a Modbus RTU transport layer instance with
 * stub callback functions:
 * \code
 * static void enable_transmission (int level)
 * {
 *    // Turn transceiver or or off, if possible.
 * }
 *
 * static void init_rtu_timers (uint32_t t1p5_us, uint32_t t3p5_us)
 * {
 *    // Set \a t1p5_us as the timeout for the T1P5 timer (if required).
 *    // Set \a t3p5_us as the timeout for the T3P5 timer (if required).
 * }
 *
 * static void start_rtu_timers (
 *    void (*t1p5_expired) (void * arg),
 *    void (*t3p5_expired) (void * arg),
 *    void * arg)
 * {
 *    // Set \a t1p5_expired as the timeout callback for the T1P5 one-shot timer.
 *    // Set \a t3p5_expired as the timeout callback for the T3P5 one-shot timer.
 *    // Start the T1P5 and T3P5 one-shot timers.
 *    // This could also be managed by hardware
 *    // directly if suitable hardware support exists.
 * }
 *
 * mb_transport_t * create_rtu_layer (void)
 * {
 *    mb_transport_t * rtu;
 *    static const mb_rtu_serial_cfg_t serial_cfg =
 *    {
 *       .baudrate = 115200,
 *       .parity = NONE,
 *    };
 *    static const mb_rtu_cfg_t rtu_cfg =
 *    {
 *       .serial = "/sio1",
 *       .serial_cfg = &serial_cfg,
 *       .tx_enable = enable_transmission,
 *       .tmr_init = init_rtu_timers,
 *       .tmr_start = start_rtu_timers,
 *    };
 *
 *    rtu = mb_rtu_init (&rtu_cfg);
 *
 *    return rtu;
 * }
 * \endcode
 *
 * \see mbus_create() and mb_slave_init().
 *
 * \param cfg           RTU layer configuration
 *
 * \return Handle to be used in further operations
 */
MB_EXPORT mb_transport_t * mb_rtu_init (const mb_rtu_cfg_t * cfg);

#ifdef __cplusplus
}
#endif

#endif /* MB_RTU_H */
