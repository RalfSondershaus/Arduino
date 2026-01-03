 /**
  * @file test.cpp
  * @author Ralf Sondershaus
  *
  * @descr Google Test for Gen/Rte/Rte.h
  *
  * @copyright Copyright 2022 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <Rte/Rte.h>
#include <gtest/gtest.h>

static unsigned long Arduino_Stub_MicrosReturnValue;
static unsigned long Arduino_Stub_MillisReturnValue;

/// Return current time [us]
unsigned long micros() { return Arduino_Stub_MicrosReturnValue; }
/// Return current time [ms]
unsigned long millis() { return Arduino_Stub_MillisReturnValue; }

// --------------------------------------------------------------------------------------------
/// Test case for runable init and run
// --------------------------------------------------------------------------------------------
TEST(Ut_Rte, init_and_run_1)
{
  Arduino_Stub_MicrosReturnValue = 0;
  rte::start();
  EXPECT_EQ(rte::a1.ulCallsInit, static_cast<uint32>(1));
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(0));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(0));
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(1));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(0));
  Arduino_Stub_MicrosReturnValue = 1000U;
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(1));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(1));
  Arduino_Stub_MicrosReturnValue = 10000U;
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(2));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(1));
  Arduino_Stub_MicrosReturnValue = 20000U;
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(3));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(1));
  Arduino_Stub_MicrosReturnValue = 21000U;
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(3));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(2));
}

// --------------------------------------------------------------------------------------------
/// Test case for SR interface
/// - write to a SR port
/// - read from a SR port
// --------------------------------------------------------------------------------------------
TEST(Ut_Rte, interface_sr_1)
{
  uint32_t ulVal;

  Arduino_Stub_MicrosReturnValue = 0;
  rte::start();
  EXPECT_EQ(rte::ifca::write(1U), rte::ret_type::OK);
  EXPECT_EQ(rte::ifca::read(ulVal), rte::ret_type::OK);
  EXPECT_EQ(ulVal, 1U);
  EXPECT_EQ(rte::ifca::write(1000U), rte::ret_type::OK);
  EXPECT_EQ(rte::ifca::read(ulVal), rte::ret_type::OK);
  EXPECT_EQ(ulVal, 1000U);
}
