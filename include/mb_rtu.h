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
 * \addtogroup mb_rtu Modbus RTU data layer
 * \{
 */

#ifndef MB_RTU_H
#define MB_RTU_H

#ifdef __cplusplus
extern "C" {
#endif

#include "mb_transport.h"
#include "mb_export.h"

#include <stdint.h>

typedef struct mb_rtu_serial_cfg
{
   int baudrate;
   enum
   {
      ODD,
      EVEN,
      NONE
   } parity;
} mb_rtu_serial_cfg_t;

typedef struct mb_rtu_cfg
{
   /**
    * Slave ID to use
    */
   uint8_t id;

   /**
    * Serial port to use, e.g. "/sio0"
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
    * This function should initialise the T1P5 and T3P5 timers, using
    * the given timeouts.
    *
    * Note that it may be possible to use a single timer if it has at
    * least two match values.
    *
    * \param t1p5               T1P5 timeout [us]
    * \param t3p5               T3P5 timeout [us]
    */
   void (*tmr_init) (uint32_t t1p5, uint32_t t3p5);

   /**
    * This function should start the T1P5 and T3P5 timers.
    *
    * \param t1p5_expired       function to be called when T1P5 expires
    * \param t3p5_expired       function to be called when T3P5 expires
    * \param arg                t1p5_expired and t3p5_expired argument
    */
   void (*tmr_start) (
      void (*t1p5_expired) (void * arg),
      void (*t3p5_expired) (void * arg),
      void * arg);
} mb_rtu_cfg_t;

typedef struct mb_rtu mb_rtu_t;

/**
 * Reconfigure the Modbus RTU serial parameters. Calling this function
 * reconfigures the serial port and calculates the T1P5 and T3P5
 * timers.
 *
 * \param rtu           handle
 * \param cfg           Serial port configuration
 */
MB_EXPORT void mb_rtu_serial_cfg (
   mb_transport_t * rtu,
   const mb_rtu_serial_cfg_t * serial_cfg);

/**
 * Initialise and configure the Modbus RTU data layer.
 *
 * \param cfg           RTU layer configuration
 *
 * \return handle to be used in further operations
 */
MB_EXPORT mb_transport_t * mb_rtu_init (const mb_rtu_cfg_t * cfg);

#ifdef __cplusplus
}
#endif

#endif /* MB_RTU_H */

/**
 * \}
 */
