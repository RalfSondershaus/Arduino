 /**
  * @file Test.cpp
  * 
  * @author Ralf Sondershaus
  *
  * @brief Unit test for Gen/Rte/Rte.h
  *
  * @copyright Copyright 2022 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <unity_adapt.h>
#include <Hal/Timer.h>
#include <Rte/Rte.h>

/**
 * @brief Specialization of EXPECT_EQ for ifc_base::ret_type
 * 
 * @see Src/Gen/Rte/Rte_Ifc_Base.h for definition of ifc_base::ret_type
 * 
 * @param actual current value
 * @param expected expected value 
 */
template<>
void EXPECT_EQ<rte::ifc_base::ret_type, rte::ifc_base::ret_type>(rte::ifc_base::ret_type actual, rte::ifc_base::ret_type expected)
{
    EXPECT_EQ(static_cast<uint8>(actual), static_cast<uint8>(expected));
}

// --------------------------------------------------------------------------------------------
/// Test case for runable init and run
// --------------------------------------------------------------------------------------------
TEST(Ut_Rte, init_and_run_1)
{
  hal::stubs::micros = 0U;
  rte::start();
  EXPECT_EQ(rte::a1.ulCallsInit, static_cast<uint32>(1));
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(0));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(0));
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(1));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(0));
  hal::stubs::micros = 1000U;
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(1));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(1));
  hal::stubs::micros = 10000U;
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(2));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(1));
  hal::stubs::micros = 20000U;
  rte::exec();
  EXPECT_EQ(rte::a1.ulCallsCyc, static_cast<uint32>(3));
  EXPECT_EQ(rte::b1.ulCallsCyc, static_cast<uint32>(1));
  hal::stubs::micros = 21000U;
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
  uint16_t val;

  hal::stubs::micros = 0U;
  rte::start();
  EXPECT_EQ(rte::ifc_uint16::write(1U), rte::ifc_base::OK);
  EXPECT_EQ(rte::ifc_uint16::read(val), rte::ifc_base::OK);
  EXPECT_EQ(val, static_cast<uint16_t>(1U));
  EXPECT_EQ(rte::ifc_uint16::write(1000U), rte::ifc_base::OK);
  EXPECT_EQ(rte::ifc_uint16::read(val), rte::ifc_base::OK);
  EXPECT_EQ(val, static_cast<uint16_t>(1000U));
}

/** 
 * @brief Intended to be called before each test.
 */
void setUp(void)
{
}

/** 
 * @brief Intended to be called after each test.
 */
void tearDown(void)
{
}

/**
 * @brief Test setup function, called once at the beginning as part of Arduino's setup().
 */
void test_setup(void)
{
}

/**
 * @brief Test setup function, called once at the beginning as part of Arduino's setup().
 * 
 * @return bool false to stop execution (relevant on Windows)
 */
bool test_loop(void)
{
  UNITY_BEGIN();

  RUN_TEST(init_and_run_1);
  RUN_TEST(interface_sr_1);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
