/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2019 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#ifndef TEST_UTIL_H
#define TEST_UTIL_H

#include <gtest/gtest.h>
#include "mocks.h"
#include "options.h"
#include "mb_pdu.h"

class TestBase : public ::testing::Test
{
 protected:
   virtual void SetUp()
   {
      /* Reset mock call counters */
      mock_mb_pdu_tx_calls = 0;
      mock_mb_pdu_rx_calls = 0;
   }
};

template <typename T> std::string FormatHex (T value)
{
   std::stringstream ss;
   uint8_t w = 2 * sizeof (T);
   ss << "0x" << std::setfill ('0') << std::setw (w) << std::hex
      << std::uppercase;
   ss << static_cast<unsigned int> (value);
   return ss.str();
}

template <typename T, size_t size>
::testing::AssertionResult ArraysMatch (
   const T (&expected)[size],
   const T (&actual)[size])
{
   for (size_t i (0); i < size; ++i)
   {
      if (expected[i] != actual[i])
      {
         return ::testing::AssertionFailure()
                << "actual[" << i << "] (" << FormatHex (actual[i])
                << ") != expected[" << i << "] (" << FormatHex (expected[i])
                << ")";
      }
   }

   return ::testing::AssertionSuccess();
}

template <typename T> std::string FormatVector (std::vector<T> vector)
{
   std::stringstream ss;

   if (vector.size() == 0)
   {
      ss << "{ empty }";
      return ss.str();
   }

   ss << "{ ";
   for (size_t i = 0; i < vector.size(); i++)
   {
      ss << FormatHex (vector[i]);
      if (i != vector.size() - 1)
         ss << ", ";
   }
   ss << " }";

   return ss.str();
}

template <typename T>
testing::AssertionResult VectorsMatch (
   const char * expected_expr,
   const char * actual_expr,
   std::vector<T> expected,
   std::vector<T> actual)
{
   if (actual == expected)
      return testing::AssertionSuccess();

   std::stringstream ss, msg;
   std::string expected_str, actual_str;

   msg << "Expected equality of these values:\n";

   msg << "  " << actual_expr << "\n";
   msg << "  Which is: " << FormatVector (actual) << "\n";

   msg << "  " << expected_expr << "\n";
   msg << "  Which is: " << FormatVector (expected) << "\n";

   return testing::AssertionFailure() << msg.str();
}
#endif /* TEST_UTIL_H */
