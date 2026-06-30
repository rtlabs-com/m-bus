/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2026 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "mb_crc.h"

#include "options.h"
#include <gtest/gtest.h>
#include <array>

#include "test_util.h"

// Test fixtures


class MbCrcTest : public TestBase
{
 protected:
   virtual void SetUp()
   {
      TestBase::SetUp();

   }
};

TEST_F (MbCrcTest, Complete)
{
   crc_t crc = 0xFFFF;
   crc = mb_crc ((const uint8_t*)"123456789", 9, crc);
   EXPECT_EQ (crc, 0x4B37);
}


TEST_F (MbCrcTest, Update)
{
   crc_t crc = 0xFFFF;
   crc = mb_crc ((const uint8_t*)"12345", 5, crc);
   crc = mb_crc ((const uint8_t*)"6789", 4, crc);
   EXPECT_EQ (crc, 0x4B37);
}
