/*********************************************************************
 *        _       _         _
 *  _ __ | |_  _ | |  __ _ | |__   ___
 * | '__|| __|(_)| | / _` || '_ \ / __|
 * | |   | |_  _ | || (_| || |_) |\__ \
 * |_|    \__|(_)|_| \__,_||_.__/ |___/
 *
 * www.rt-labs.com
 * Copyright 2020 rt-labs AB, Sweden.
 *
 * This software is dual-licensed under GPLv3 and a commercial
 * license. See the file LICENSE.md distributed with this software for
 * full license information.
 ********************************************************************/

#include "mb_slave.h"

#include "options.h"
#include "osal.h"
#include <gtest/gtest.h>

#include "mocks.h"
#include "test_util.h"

using namespace std;
using namespace testing;

extern "C" void __assert_func (
   const char * file,
   int line,
   const char * func,
   const char * expr)
{
   printf ("FAILED ASSERTION \"%s\" in %s @ %s:%d\n\n", expr, func, file, line);
   CC_ASSERT (0);
}

// Test fixture

uint8_t coils[2];
uint16_t hold[4];

extern "C" int coil_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t bit = address + offset;
      int value;

      value = mb_slave_bit_get (coils, bit);
      mb_slave_bit_set (data, offset, value);
   }
   return 0;
}

extern "C" int coil_set (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t bit = address + offset;
      int value;

      value = mb_slave_bit_get (data, offset);
      mb_slave_bit_set (coils, bit, value);
   }
   return 0;
}

extern "C" int input_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      mb_slave_bit_set (data, offset, 1);
   }
   return 0;
}

extern "C" int hold_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t reg = address + offset;

      mb_slave_reg_set (data, offset, hold[reg]);
   }
   return 0;
}

extern "C" int hold_set (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      uint32_t reg = address + offset;

      hold[reg] = mb_slave_reg_get (data, offset);
   }
   return 0;
}

extern "C" int reg_get (uint16_t address, uint8_t * data, size_t quantity)
{
   uint16_t offset;

   for (offset = 0; offset < quantity; offset++)
   {
      mb_slave_reg_set (data, offset, 0x1100 | (offset & 0xFF));
   }
   return 0;
}

extern "C" int ping (uint8_t * data, size_t rx_count)
{
   for (size_t i = 0; i < rx_count; i++)
   {
      data[i] |= 0x80;
   }
   return rx_count;
}

extern "C" const mb_vendor_func_t vendor_funcs[] = {
   {101, ping},
};

extern "C" const mb_iomap_t slave_iomap = {
   .coils = {16, coil_get, coil_set},            // 16 coils
   .inputs = {2, input_get, NULL},               // 2 input status bits
   .holding_registers = {4, hold_get, hold_set}, // 4 holding registers
   .input_registers = {5, reg_get, NULL},        // 5 input registers
   .num_vendor_funcs = NELEMENTS (vendor_funcs), // 1 vendor function
   .vendor_funcs = vendor_funcs,
};

extern "C" const mb_slave_cfg_t slave_cfg = {
   .id = 2, // Slave ID: 2
   .priority = 15,
   .stack_size = 2048,
   .iomap = &slave_iomap,
};

class MbSlaveTest : public TestBase
{
 protected:
   virtual void SetUp()
   {
      TestBase::SetUp();

      slave.iomap = &slave_iomap;
      slave.transport = NULL;
      slave.id = 2;
      slave.running = 1;

      transaction.data = buffer;

      coils[0] = 0x00;
      coils[1] = 0x00;

      hold[0] = 0x1234;
      hold[1] = 0x5678;
      hold[2] = 0x55AA;
      hold[3] = 0xAA55;
   }

   mb_slave_t slave;
   pdu_txn_t transaction;
   uint8_t buffer[MAX_PDU_SIZE];
};

// Parameterized test fixtures

class MbSlaveTestRead
    : public MbSlaveTest,
      public WithParamInterface<tuple<vector<uint8_t>, vector<uint8_t>>>
{
};

class MbSlaveTestWrite
    : public MbSlaveTest,
      public WithParamInterface<
         tuple<vector<uint8_t>, vector<uint8_t>, vector<uint8_t>, vector<uint16_t>>>
{
};

// Tests

TEST_P (MbSlaveTestRead, MbSlaveTestReadResponse)
{
   vector<uint8_t> request = get<0> (GetParam());
   vector<uint8_t> expected_tx = get<1> (GetParam());

   mock_mb_pdu_rx_data = &request[0];
   mock_mb_pdu_rx_size = request.size();
   mock_mb_pdu_rx_result = request.size();

   mb_slave_handle_request (&slave, &transaction);

   vector<uint8_t> tx_data (
      mock_mb_pdu_tx_data,
      mock_mb_pdu_tx_data + mock_mb_pdu_tx_size);

   EXPECT_PRED_FORMAT2 (VectorsMatch, tx_data, expected_tx);
}

// clang-format off
INSTANTIATE_TEST_CASE_P (
   ReadTests,
   MbSlaveTestRead,
   Values (
      // Read coils
      make_tuple (
         vector<uint8_t>{0x01, 0x00, 0x0A, 0x00, 0x06},
         vector<uint8_t>{0x01, 0x01, 0x00}
      ),
      // Read inputs
      make_tuple (
         vector<uint8_t>{0x02, 0x00, 0x00, 0x00, 0x02},
         vector<uint8_t>{0x02, 0x01, 0x03}
      ),
      // Read input registers
      make_tuple (
         vector<uint8_t>{0x04, 0x00, 0x00, 0x00, 0x05},
         vector<uint8_t>{0x04, 0x0a, 0x11, 0x00, 0x11, 0x01, 0x11, 0x02, 0x11, 0x03, 0x11, 0x04}
      ),
      // Read holding registers
      make_tuple (
         vector<uint8_t>{0x03, 0x00, 0x00, 0x00, 0x04},
         vector<uint8_t>{0x03, 0x08, 0x12, 0x34, 0x56, 0x78, 0x55, 0xaa, 0xaa, 0x55}
      ),
      // Read coils address error
      make_tuple (
         vector<uint8_t>{0x01, 0x00, 0x0A, 0x00, 0x07},
         vector<uint8_t>{0x81, 0x02}
      ),
      // Read inputs address error
      make_tuple (
         vector<uint8_t>{0x02, 0x00, 0x00, 0x00, 0x03},
         vector<uint8_t>{0x82, 0x02}
      ),
      // Read input registers address error
      make_tuple (
         vector<uint8_t>{0x04, 0x00, 0x00, 0x00, 0x06},
         vector<uint8_t>{0x84, 0x02}
      ),
      // Read holding registers address error
      make_tuple (
         vector<uint8_t>{0x03, 0x00, 0x00, 0x00, 0x05},
         vector<uint8_t>{0x83, 0x02}
      ),
      // Read coils quantity error
      make_tuple (
         vector<uint8_t>{0x01, 0x00, 0x0A, 0x07, 0xD1},
         vector<uint8_t>{0x81, 0x03}
      ),
      // Read inputs quantity error
      make_tuple (
         vector<uint8_t>{0x02, 0x00, 0x00, 0x07, 0xD1},
         vector<uint8_t>{0x82, 0x03}
      ),
      // Read input registers quantity error
      make_tuple (
         vector<uint8_t>{0x04, 0x00, 0x00, 0x00, 0x7E},
         vector<uint8_t>{0x84, 0x03}
      ),
      // Read holding registers quantity error
      make_tuple (
         vector<uint8_t>{0x03, 0x00, 0x00, 0x00, 0x7E},
         vector<uint8_t>{0x83, 0x03}
      )
   )
);
// clang-format on

TEST_P (MbSlaveTestWrite, MbSlaveTestWriteResponseAndState)
{
   vector<uint8_t> request = get<0> (GetParam());
   vector<uint8_t> expected_tx = get<1> (GetParam());
   vector<uint8_t> expected_coils = get<2> (GetParam());
   vector<uint16_t> expected_hold = get<3> (GetParam());

   mock_mb_pdu_rx_data = &request[0];
   mock_mb_pdu_rx_size = request.size();
   mock_mb_pdu_rx_result = request.size();

   mb_slave_handle_request (&slave, &transaction);

   vector<uint8_t> tx_data (
      mock_mb_pdu_tx_data,
      mock_mb_pdu_tx_data + mock_mb_pdu_tx_size);

   EXPECT_PRED_FORMAT2 (VectorsMatch, tx_data, expected_tx);

   if (expected_coils.size() > 0)
   {
      vector<uint8_t> vect_coils (coils, coils + NELEMENTS (coils));
      EXPECT_PRED_FORMAT2 (VectorsMatch, vect_coils, expected_coils);
   }

   if (expected_hold.size() > 0)
   {
      vector<uint16_t> vect_hold (hold, hold + NELEMENTS (hold));
      EXPECT_PRED_FORMAT2 (VectorsMatch, vect_hold, expected_hold);
   }
}

// clang-format off
INSTANTIATE_TEST_CASE_P (
   WriteTests,
   MbSlaveTestWrite,
   Values (
      // Write coils bit 1
      make_tuple (
         vector<uint8_t>{0x05, 0x00, 0x01, 0xFF, 0x00}, // Request
         vector<uint8_t>{0x05, 0x00, 0x01, 0xFF, 0x00}, // Expected Tx
         vector<uint8_t>{0x02, 0x00},                   // Expected coils
         vector<uint16_t>{}                             // Expected hold
      ),
      // Write coils bits 2-12
      make_tuple (
         vector<uint8_t>{0x0F, 0x00, 0x02, 0x00, 0x0A, 0x02, 0xFF, 0x03},
         vector<uint8_t>{0x0F, 0x00, 0x02, 0x00, 0x0A},
         vector<uint8_t>{0xFC, 0x0F},
         vector<uint16_t>{}
      ),
      // Write holding register 1
      make_tuple (
         vector<uint8_t>{0x06, 0x00, 0x01, 0x11, 0x22},
         vector<uint8_t>{0x06, 0x00, 0x01, 0x11, 0x22},
         vector<uint8_t>{},
         vector<uint16_t>{0x1234, 0x1122, 0x55AA, 0xAA55}
      ),
      // Write 2 holding registers
      make_tuple (
         vector<uint8_t>{0x10, 0x00, 0x01, 0x00, 0x02, 0x04, 0x11, 0x22, 0x33, 0x44},
         vector<uint8_t>{0x10, 0x00, 0x01, 0x00, 0x02},
         vector<uint8_t>{},
         vector<uint16_t>{0x1234, 0x1122, 0x3344, 0xAA55}
      ),
      // ReadWrite 2 holding registers
      make_tuple (
         vector<uint8_t>{0x17, 0x00, 0x01, 0x00, 0x02, 0x00, 0x01, 0x00, 0x01, 0x02, 0x11, 0x22},
         vector<uint8_t>{0x17, 0x04, 0x11, 0x22, 0x55, 0xAA},
         vector<uint8_t>{},
         vector<uint16_t>{0x1234, 0x1122, 0x55AA, 0xAA55}
      )
   )
);
// clang-format on
