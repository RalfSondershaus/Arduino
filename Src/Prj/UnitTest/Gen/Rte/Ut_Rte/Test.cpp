 /**
  * @file test.cpp
  * @author Ralf Sondershaus
  *
  * @descr Google Test for Gen/Rte/Rte.h
  *
  * @copyright Copyright 2022 Ralf Sondershaus
  *
  * This program is free software: you can redistribute it and/or modify it
  * under the terms of the GNU General Public License as published by the
  * Free Software Foundation, either version 3 of the License, or (at your
  * option) any later version.
  *
  * This program is distributed in the hope that it will be useful, but
  * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
  * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
  * for more details.
  *
  * See <https://www.gnu.org/licenses/>.
  */

#include <Rte/Rte.h>
#include <gtest/gtest.h>

static uint8 ucGlobal1 = 0;

static unsigned long Arduino_Stub_MicrosReturnValue;
static unsigned long Arduino_Stub_MillisReturnValue;

/// Return current time [us]
unsigned long micros() { return Arduino_Stub_MicrosReturnValue; }
/// Return current time [ms]
unsigned long millis() { return Arduino_Stub_MillisReturnValue; }

// --------------------------------------------------------------------------------------------
/// A helper class which provides runable functions init and execute
// --------------------------------------------------------------------------------------------
class A1
{
public:
  A1() = default;
  void init() noexcept { ucGlobal1 = 0xAAU; }
  void execute() noexcept { ucGlobal1++; }
};

// --------------------------------------------------------------------------------------------
/// Test case for runable init and run
// --------------------------------------------------------------------------------------------
TEST(Ut_Rte, init_and_run_1)
{
  /// Sample class
  A1 a1;

  // --------------------------------------------------------------------------------------------
  /// RTE configuration
  // --------------------------------------------------------------------------------------------

  /// The runables of instance theBlinker
  Rte::TRunable<A1> rA1_Init(a1, &A1::init);
  Rte::TRunable<A1> rA1_Cyclic(a1, &A1::execute);

  /// One init runable and one cyclic runable
  constexpr int NCR = 1;
  constexpr int NIR = 1;

  /// The arrays to configure the RTE
  const Rte::RTE<NIR, NCR>::tCyclicCfgArray rcb_cfg_array = {
    {
      { 0U, 10000U, &rA1_Cyclic }
    }
  };
  static const Rte::RTE<NIR, NCR>::tInitArray init_array = { &rA1_Init };

  /// The RTE
  Rte::RTE<1, 1> rte(init_array, rcb_cfg_array);

  ucGlobal1 = 0U;
  Arduino_Stub_MicrosReturnValue = 0;

  rte.start();
  EXPECT_EQ(ucGlobal1, static_cast<uint8>(0xAAU));
  rte.exec1();
  EXPECT_EQ(ucGlobal1, static_cast<uint8>(0xAAU + 1U));
  rte.exec1();
  EXPECT_EQ(ucGlobal1, static_cast<uint8>(0xAAU + 1U));
  Arduino_Stub_MicrosReturnValue = 10000U;
  rte.exec1();
  EXPECT_EQ(ucGlobal1, static_cast<uint8>(0xAAU + 2U));
}

