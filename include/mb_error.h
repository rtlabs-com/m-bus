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
 * \addtogroup mb_error Modbus error codes
 * \{
 */

#ifndef MB_ERROR_H
#define MB_ERROR_H

#ifdef __cplusplus
extern "C" {
#endif

/* Modbus exceptions. A slave callback can return these. A master read
   or write operation may result in one of these exceptions. */
#define EILLEGAL_FUNCTION     -1 /**< Modbus exception ILLEGAL_FUNCTION */
#define EILLEGAL_DATA_ADDRESS -2 /**< Modbus exception ILLEGAL_DATA_ADDRESS */
#define EILLEGAL_DATA_VALUE   -3 /**< Modbus exception ILLEGAL_DATA_VALUE */
#define ESLAVE_DEVICE_FAILURE -4 /**< Modbus exception SLAVE_DEVICE_FAILURE */

/* Modbus communication errors */
#define ECRC_FAIL          -101 /**< CRC check failed */
#define EFRAME_NOK         -102 /**< Received frame not valid */
#define ESLAVE_ID          -103 /**< Unexpected slave ID */
#define ETIMEOUT           -104 /**< Receive timed out */
#define EUNKNOWN_EXCEPTION -105 /**< Modbus exception code not recognised */

static inline const char * mb_error_literal (int error)
{
   switch (error)
   {
   case EILLEGAL_FUNCTION:
      return "EILLEGAL_FUNCTION";
   case EILLEGAL_DATA_ADDRESS:
      return "EILLEGAL_DATA_ADDRESS";
   case EILLEGAL_DATA_VALUE:
      return "EILLEGAL_DATA_VALUE";
   case ESLAVE_DEVICE_FAILURE:
      return "ESLAVE_DEVICE_FAILURE";
   case ECRC_FAIL:
      return "ECRC_FAIL";
   case EFRAME_NOK:
      return "EFRAME_NOK";
   case ESLAVE_ID:
      return "ESLAVE_ID";
   case ETIMEOUT:
      return "ETIMEOUT";
   case EUNKNOWN_EXCEPTION:
      return "EUNKNOWN_EXCEPTION";
   default:
      return "Unknown error";
   }
}

#ifdef __cplusplus
}
#endif

#endif /* MB_ERROR_H */

/**
 * \}
 */
