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

#include "mbus.h"

#include "options.h"
#include "osal.h"
#include <gtest/gtest.h>

#include "mocks.h"
#include "test_util.h"

// Test fixture

class MbusTest : public TestBase
{
 protected:
   virtual void SetUp()
   {
      TestBase::SetUp();
      mbus_init (&mbus, &mbus_cfg, &transport, scratch);
   }

   mbus_cfg_t mbus_cfg = {
      .timeout = 1000,
   };
   mbus_t mbus;
   uint8_t scratch[MAX_PDU_SIZE];
   mb_transport_t transport;
};

// Tests

TEST_F (MbusTest, MbusReadCoils)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   uint8_t data[4];
   int error;
   uint8_t expected[253] = {0x01, 0x27, 0x10, 0x00, 0x04};
   uint8_t response[]    = {0x01, 0x06, 0x12, 0x34, 0x56, 0x78};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
   EXPECT_EQ (data[0], 0x12);
   EXPECT_EQ (data[1], 0x34);
   EXPECT_EQ (data[2], 0x56);
   EXPECT_EQ (data[3], 0x78);
}

TEST_F (MbusTest, MbusReadInputs)
{
   mb_address_t address = MB_ADDRESS (1, 0x2711);
   uint8_t data[4];
   int error;
   uint8_t expected[253] = {0x02, 0x27, 0x10, 0x00, 0x04};
   uint8_t response[] = {0x02, 0x06, 0x12, 0x34, 0x56, 0x78};

   mock_mb_pdu_rx_data = response;
   mock_mb_pdu_rx_size = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
   EXPECT_EQ (data[0], 0x12);
   EXPECT_EQ (data[1], 0x34);
   EXPECT_EQ (data[2], 0x56);
   EXPECT_EQ (data[3], 0x78);
}

TEST_F (MbusTest, MbusReadInputRegisters)
{
   mb_address_t address = MB_ADDRESS (3, 0x2711);
   uint16_t data[3];
   int error;
   uint8_t expected[253] = {0x04, 0x27, 0x10, 0x00, 0x03};
   uint8_t response[] = {0x04, 0x06, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

   mock_mb_pdu_rx_data = response;
   mock_mb_pdu_rx_size = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
   EXPECT_EQ (data[0], 0x1122);
   EXPECT_EQ (data[1], 0x3344);
   EXPECT_EQ (data[2], 0x5566);
}

TEST_F (MbusTest, MbusReadHoldingRegisters)
{
   mb_address_t address = MB_ADDRESS (4, 0x2711);
   uint16_t data[3];
   int error;
   uint8_t expected[253] = {0x03, 0x27, 0x10, 0x00, 0x03};
   uint8_t response[] = {0x03, 0x08, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
   EXPECT_EQ (data[0], 0x1122);
   EXPECT_EQ (data[1], 0x3344);
   EXPECT_EQ (data[2], 0x5566);
}

TEST_F (MbusTest, MbusReadShouldValidateQuantity)
{
   mb_address_t address;
   int error;

   // Max coils exceeded
   address = MB_ADDRESS (0, 0x1234);
   error   = mbus_read (&mbus, 1, address, 2001, NULL);
   EXPECT_EQ (error, -1);

   // Max inputs exceeded
   address = MB_ADDRESS (1, 0x1234);
   error   = mbus_read (&mbus, 1, address, 2001, NULL);
   EXPECT_EQ (error, -1);

   // Max input registers exceeded
   address = MB_ADDRESS (3, 0x1234);
   error   = mbus_read (&mbus, 1, address, 126, NULL);
   EXPECT_EQ (error, -1);

   // Max holding registers exceeded
   address = MB_ADDRESS (4, 0x1234);
   error   = mbus_read (&mbus, 1, address, 126, NULL);
   EXPECT_EQ (error, -1);
}

TEST_F (MbusTest, MbusReadShouldHandleRxError)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   uint8_t data[1];
   int error;

   mock_mb_pdu_rx_data   = NULL;
   mock_mb_pdu_rx_size   = 0;
   mock_mb_pdu_rx_result = -1;

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (error, -1);
}

TEST_F (MbusTest, MbusReadShouldHandleRxException)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   uint8_t data[4];
   int error;
   uint8_t response[] = {0x81, 0x00};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   response[1] = 1;

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_FUNCTION, error);

   response[1] = 2;

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_DATA_ADDRESS, error);

   response[1] = 3;

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_DATA_VALUE, error);

   response[1] = 4;

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (ESLAVE_DEVICE_FAILURE, error);

   response[1] = 99;

   error = mbus_read (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EUNKNOWN_EXCEPTION, error);
}

TEST_F (MbusTest, MbusReadShouldDenyBroadcast)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   uint8_t data[4];
   int error;

   error = mbus_read (&mbus, 0, address, NELEMENTS (data), data);
   EXPECT_EQ (error, -1);
   EXPECT_EQ (mock_mb_pdu_tx_calls, 0u);
}

TEST_F (MbusTest, MbusWriteSingleCoil)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   int error;
   uint8_t expected[253] = {0x05, 0x27, 0x10, 0xff, 0x00};
   uint8_t response[]    = {0x05};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   error = mbus_write_single (&mbus, 1, address, 1);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
}

TEST_F (MbusTest, MbusWriteSingleHoldingRegister)
{
   mb_address_t address = MB_ADDRESS (4, 0x2711);
   int error;
   uint8_t expected[253] = {0x06, 0x27, 0x10, 0x55, 0xAA};
   uint8_t response[]    = {0x06};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   error = mbus_write_single (&mbus, 1, address, 0x55AA);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
}

TEST_F (MbusTest, MbusWriteSingleShouldHandleRxError)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   int error;

   mock_mb_pdu_rx_data   = NULL;
   mock_mb_pdu_rx_size   = 0;
   mock_mb_pdu_rx_result = -1;

   error = mbus_write_single (&mbus, 1, address, 0x42);
   EXPECT_EQ (error, -1);
}

TEST_F (MbusTest, MbusWriteSingleShouldHandleRxException)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   int error;
   uint8_t response[] = {0x81, 0x00};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   response[1] = 1;

   error = mbus_write_single (&mbus, 1, address, 0x55AA);
   EXPECT_EQ (EILLEGAL_FUNCTION, error);

   response[1] = 2;

   error = mbus_write_single (&mbus, 1, address, 0x55AA);
   EXPECT_EQ (EILLEGAL_DATA_ADDRESS, error);

   response[1] = 3;

   error = mbus_write_single (&mbus, 1, address, 0x55AA);
   EXPECT_EQ (EILLEGAL_DATA_VALUE, error);

   response[1] = 4;

   error = mbus_write_single (&mbus, 1, address, 0x55AA);
   EXPECT_EQ (ESLAVE_DEVICE_FAILURE, error);

   response[1] = 99;

   error = mbus_write_single (&mbus, 1, address, 0x55AA);
   EXPECT_EQ (EUNKNOWN_EXCEPTION, error);
}

TEST_F (MbusTest, MbusWriteSingleShouldNotExpectResponseOnBroadcast)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   int error;

   error = mbus_write_single (&mbus, 0, address, 0x55AA);
   EXPECT_EQ (error, 0);
   EXPECT_EQ (mock_mb_pdu_rx_calls, 0u);
}

TEST_F (MbusTest, MbusWriteCoil)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   int error;
   uint8_t data[2];
   uint8_t expected[253] = {0x0F, 0x27, 0x10, 0x00, 0x0C, 0x02, 0x55, 0xAA};
   uint8_t response[]    = {0x0F};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   data[0] = 0x55;
   data[1] = 0xAA;
   error   = mbus_write (&mbus, 1, address, 12, data);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
}

TEST_F (MbusTest, MbusWriteHoldingRegister)
{
   mb_address_t address = MB_ADDRESS (4, 0x2711);
   int error;
   uint16_t data[2];
   uint8_t expected[253] =
      {0x10, 0x27, 0x10, 0x00, 0x02, 0x04, 0x55, 0xAA, 0x11, 0x22};
   uint8_t response[] = {0x06};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   data[0] = 0x55AA;
   data[1] = 0x1122;
   error   = mbus_write (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (error, 0);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
}

TEST_F (MbusTest, MbusWriteShouldHandleRxError)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   uint16_t data[2];
   int error;

   mock_mb_pdu_rx_data   = NULL;
   mock_mb_pdu_rx_size   = 0;
   mock_mb_pdu_rx_result = -1;

   data[0] = 0x55AA;
   data[1] = 0x1122;
   error   = mbus_write (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (error, -1);
}

TEST_F (MbusTest, MbusWriteShouldValidateQuantity)
{
   mb_address_t address;
   int error;

   // Max coils exceeded
   address = MB_ADDRESS (0, 0x1234);
   error   = mbus_write (&mbus, 1, address, 1969, NULL);
   EXPECT_EQ (error, -1);

   // Max holding registers exceeded
   address = MB_ADDRESS (4, 0x1234);
   error   = mbus_write (&mbus, 1, address, 124, NULL);
   EXPECT_EQ (error, -1);
}

TEST_F (MbusTest, MbusWriteShouldHandleRxException)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   uint16_t data[2];
   int error;
   uint8_t response[] = {0x81, 0x00};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   data[0] = 0x55AA;
   data[1] = 0x1122;

   response[1] = 1;

   error = mbus_write (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_FUNCTION, error);

   response[1] = 2;

   error = mbus_write (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_DATA_ADDRESS, error);

   response[1] = 3;

   error = mbus_write (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_DATA_VALUE, error);

   response[1] = 4;

   error = mbus_write (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (ESLAVE_DEVICE_FAILURE, error);

   response[1] = 99;

   error = mbus_write (&mbus, 1, address, NELEMENTS (data), data);
   EXPECT_EQ (EUNKNOWN_EXCEPTION, error);
}

TEST_F (MbusTest, MbusWriteShouldNotExpectResponseOnBroadcast)
{
   mb_address_t address = MB_ADDRESS (0, 0x2711);
   uint16_t data[2];
   int error;

   data[0] = 0x55AA;
   data[1] = 0x1122;

   error = mbus_write (&mbus, 0, address, NELEMENTS (data), data);
   EXPECT_EQ (error, 0);
   EXPECT_EQ (mock_mb_pdu_rx_calls, 0u);
}

TEST_F (MbusTest, MbusLoopback)
{
   int error;
   uint8_t data[]        = {0x11, 0x22, 0x33, 0x44};
   uint8_t expected[253] = {0x08, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};
   uint8_t response[]    = {0x08, 0x00, 0x00, 0x11, 0x22, 0x33, 0x44};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   error = mbus_loopback (&mbus, 1, NELEMENTS (data), data);
   EXPECT_EQ (error, (int)sizeof (response));
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
}

TEST_F (MbusTest, MbusLoopbackShouldValidateQuantity)
{
   int error;

   // Max size exceeded
   error = mbus_loopback (&mbus, 1, 251, NULL);
   EXPECT_EQ (error, -1);
}

TEST_F (MbusTest, MbusLoopbackShouldHandleRxError)
{
   uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
   int error;

   mock_mb_pdu_rx_data   = NULL;
   mock_mb_pdu_rx_size   = 0;
   mock_mb_pdu_rx_result = -1;

   error = mbus_loopback (&mbus, 1, NELEMENTS (data), data);
   EXPECT_EQ (error, -1);
}

TEST_F (MbusTest, MbusLoopbackShouldHandleRxException)
{
   uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
   int error;
   uint8_t response[] = {0x81, 0x00};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   response[1] = 1;

   error = mbus_loopback (&mbus, 1, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_FUNCTION, error);

   response[1] = 3;

   error = mbus_loopback (&mbus, 1, NELEMENTS (data), data);
   EXPECT_EQ (EILLEGAL_DATA_VALUE, error);

   response[1] = 4;

   error = mbus_loopback (&mbus, 1, NELEMENTS (data), data);
   EXPECT_EQ (ESLAVE_DEVICE_FAILURE, error);

   response[1] = 99;

   error = mbus_loopback (&mbus, 1, NELEMENTS (data), data);
   EXPECT_EQ (EUNKNOWN_EXCEPTION, error);
}

TEST_F (MbusTest, MbusLoopbackShouldNotExpectResponseOnBroadcast)
{
   uint8_t data[] = {0x11, 0x22, 0x33, 0x44};
   int error;

   error = mbus_loopback (&mbus, 0, NELEMENTS (data), data);
   EXPECT_EQ (error, 0);
   EXPECT_EQ (mock_mb_pdu_rx_calls, 0u);
}

TEST_F (MbusTest, MbusSendMsg)
{
   int error;
   uint8_t data[]        = {0x11, 0x22, 0x33, 0x44};
   uint8_t expected[253] = {0x11, 0x22, 0x33, 0x44};

   error = mbus_send_msg (&mbus, 1, data, NELEMENTS (data));
   EXPECT_EQ (error, 0);
   EXPECT_EQ (mock_mb_pdu_tx_calls, 1u);
   EXPECT_TRUE (ArraysMatch (expected, mock_mb_pdu_tx_data));
}

TEST_F (MbusTest, MbusGetMsg)
{
   int count;
   uint8_t data[4];
   uint8_t response[] = {0x11, 0x22, 0x33, 0x44};

   mock_mb_pdu_rx_data   = response;
   mock_mb_pdu_rx_size   = sizeof (response);
   mock_mb_pdu_rx_result = sizeof (response);

   count = mbus_get_msg (&mbus, 1, data, NELEMENTS (data));
   EXPECT_EQ (count, (int)sizeof (data));
   EXPECT_EQ (mock_mb_pdu_rx_calls, 1u);
   EXPECT_TRUE (ArraysMatch (data, response));
}
