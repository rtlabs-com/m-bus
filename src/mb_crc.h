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

#ifndef MB_CRC_H
#define MB_CRC_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

typedef uint16_t crc_t;

crc_t mb_crc (const uint8_t * buffer, uint8_t len, crc_t preload);

#ifdef __cplusplus
}
#endif

#endif /* MB_CRC_H */
