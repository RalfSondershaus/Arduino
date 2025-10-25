/**
 * @file Ut_Sstream_Performance/Test.cpp
 *
 * @brief Unit tests to measure run time of project util::sstream
 *
 * @copyright Copyright 2025 Ralf Sondershaus
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

#include <stdint.h>
#include <Hal/Serial.h>
#include <unity_adapt.h>
#include <Test/Logger.h>
#include <Util/Sstream.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

/**
 * @brief Intermediate variable for performance test
 */
uint16_t un;

/**
 * @brief Test performance of formatted input of uint16
 * 
 */
TEST(Ut_Sstream, Formatted_input_sstream_uint16)
{
    using istringstream_t = util::basic_istringstream<16, char>;

    constexpr int nr_rep = 1000; /**< Number of repetitions for performance testing */

    uint32 t1;
    uint32 td;
    t1 = micros();

    for (int i = 0; i < nr_rep; i++)
    {
        istringstream_t stream("12345");
        stream >> un;
    }
    td = micros() - t1;
    EXPECT_EQ(un, uint16{ 12345 });
    hal::serial::print("stream >> un ");
    hal::serial::println(td / nr_rep);
}

/**
 * @brief Test performance of formatted input of uint16
 * 
 */
TEST(Ut_Sstream, Formatted_input_stdlib_strtoul_uint16)
{
    constexpr int nr_rep = 1000; /**< Number of repetitions for performance testing */

    uint32 t1;
    uint32 td;
    t1 = micros();

    for (int i = 0; i < nr_rep; i++)
    {
        un= strtoul("12345", nullptr, 10);
    }
    td = micros() - t1;
    EXPECT_EQ(un, uint16{ 12345 });
    hal::serial::print("stream >> un ");
    hal::serial::println(td / nr_rep);
}

/**
 * @brief Test performance of formatted input of uint16
 * 
 */
TEST(Ut_Sstream, Formatted_input_stdlib_atol_uint16)
{
    constexpr int nr_rep = 1000; /**< Number of repetitions for performance testing */

    uint32 t1;
    uint32 td;
    t1 = micros();

    for (int i = 0; i < nr_rep; i++)
    {
        un = atol("12345");
    }
    td = micros() - t1;
    EXPECT_EQ(un, uint16{ 12345 });
    hal::serial::print("stream >> un ");
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

  RUN_TEST(Formatted_input_sstream_uint16);
  RUN_TEST(Formatted_input_stdlib_strtoul_uint16);
  RUN_TEST(Formatted_input_stdlib_atol_uint16);
 
  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}