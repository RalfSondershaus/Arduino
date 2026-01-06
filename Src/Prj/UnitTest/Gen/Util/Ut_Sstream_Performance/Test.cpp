/**
 * @file Ut_Sstream_Performance/Test.cpp
 *
 * @brief Unit tests to measure run time of project util::sstream
 *
 * @copyright Copyright 2025 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifdef ARDUINO
#include <Arduino.h>
#endif

#include <stdint.h>
#include <Hal/Serial.h>
#include <unity_adapt.h>
#include <Test/Logger.h>
#include <Util/Sstream.h>
#include <Util/String.h>

/**
 * @brief Intermediate variable for performance test
 */
uint16_t un;

uint32 td_ext_1;
uint32 td_ext_2;

/**
 * @brief Test performance of formatted input of uint16
 * 
 * With constructor: 236 us
 * Without constructor: 218 us
 */
TEST(Ut_Sstream, Formatted_input_util_sstream_uint16)
{
    using stringstream = util::basic_istringstream<16, char>;

    constexpr int nr_rep = 1000; /**< Number of repetitions for performance testing */

    uint32 t1;
    uint32 t2;
    uint32 td_constr;
    uint32 td_conv;

    un = 0;
    td_constr = 0;
    td_conv = 0;
    td_ext_1 = 0;
    td_ext_2 = 0;

    for (int i = 0; i < nr_rep; i++)
    {
        t1 = micros();
        stringstream stream("12345");
        t2 = micros();
        td_constr += t2 - t1;
        stream >> un;
        td_conv += micros() - t2;
    }


    EXPECT_EQ(un, uint16{ 12345 });

    hal::serial::print("stream >> un constr:");
    hal::serial::print(td_constr / nr_rep);
    hal::serial::print(" us, conv:");
    hal::serial::print(td_conv / nr_rep);
    hal::serial::print(" us, td_ext_1: ");
    hal::serial::print(td_ext_1 / nr_rep);
    hal::serial::print(" us, td_ext_2: ");
    hal::serial::print(td_ext_2 / nr_rep);
    hal::serial::println(" us");
}

/**
 * @brief Test performance of formatted input of uint16 with util::stoui<uint16>
 * 
 */
TEST(Ut_Sstream, Formatted_input_util_strtoi_uint16)
{
    using string_type = util::basic_string<16, char>;

    constexpr int nr_rep = 1000; /**< Number of repetitions for performance testing */

    uint32 t1;
    uint32 td;
    uint16 tmp;

    string_type str("12345");
    
    un = 0;
    tmp = 0;

    t1 = micros();

    for (int i = 0; i < nr_rep; i++)
    {
        // Add to prevent optimizer from removing calls of this loop
        un += util::stoui<uint16>(str);
    }

    td = micros() - t1;

    for (int i = 0; i < nr_rep; i++)
    {
        tmp += 12345;
    }

    EXPECT_EQ(un, tmp);

    hal::serial::print("util::stoui ");
    hal::serial::println(td / nr_rep);
}

/**
 * @brief Test performance of formatted input of uint16 with gcc's strtul
 * 
 */
TEST(Ut_Sstream, Formatted_input_stdlib_strtoul_uint16)
{
    constexpr int nr_rep = 1000; /**< Number of repetitions for performance testing */

    uint32 t1;
    uint32 td;
    uint16 tmp;

    un = 0;

    t1 = micros();
    
    for (int i = 0; i < nr_rep; i++)
    {
        // Add to prevent optimizer from removing calls of this loop
        un += strtoul("12345", nullptr, 10);
    }

    td = micros() - t1;

    for (int i = 0; i < nr_rep; i++)
    {
        tmp += 12345;
    }

    EXPECT_EQ(un, tmp);

    hal::serial::print("stdlib_strtoul ");
    hal::serial::println(td / nr_rep);
}

/**
 * @brief Test performance of formatted input of uint16 with gcc's atol
 * 
 */
TEST(Ut_Sstream, Formatted_input_stdlib_atol_uint16)
{
    constexpr int nr_rep = 1000; /**< Number of repetitions for performance testing */

    uint32 t1;
    uint32 td;
    uint16 tmp;

    un = 0;
    tmp = 0;

    t1 = micros();

    for (int i = 0; i < nr_rep; i++)
    {
        // Add to prevent optimizer from removing calls of this loop
        un += atol("12345");
    }

    td = micros() - t1;

    for (int i = 0; i < nr_rep; i++)
    {
        tmp += 12345;
    }

    EXPECT_EQ(un, tmp);
    hal::serial::print("stdlib_atol ");
    hal::serial::println(td / nr_rep);
}

/**
 * @brief 
 * 
 */
void setUp(void)
{
}

/**
 * @brief Is called after each test case
 * 
 */
void tearDown(void)
{
}

/**
 * @brief Is called once the Arduino starts (called in Arduino's setup() function)
 */
void test_setup(void)
{
}

/**
 * @brief 
 * 
 */
bool test_loop(void)
{
  UNITY_BEGIN();

  RUN_TEST(Formatted_input_util_sstream_uint16);
  RUN_TEST(Formatted_input_util_strtoi_uint16);
  RUN_TEST(Formatted_input_stdlib_strtoul_uint16);
  RUN_TEST(Formatted_input_stdlib_atol_uint16);
 
  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}