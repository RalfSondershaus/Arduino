/**
  * @file unity_adapt.h
  * @author Ralf Sondershaus
  *
  * @brief Useful macros to adapt Unity to our needs
  * 
  * @copyright Copyright 2024 Ralf Sondershaus
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
  * You should have received a copy of the GNU General Public License
  * along with this program.  If not, see <https://www.gnu.org/licenses/>.
  */


#ifndef UNITY_ADAPT_H
#define UNITY_ADAPT_H

#include <unity.h>
#include <Std_Types.h>
#include <Util/Ios_Type.h>
//#include <iostream>

/// @brief Introduce Google Test like expect macros
/// @tparam T Type to be checked such as uint8, int, size_t
/// @param actual current value
/// @param expected expected value
template<typename T1, typename T2>
void EXPECT_EQ(T1 actual, T2 expected)
{
  // not implemented yet
  // std::cout << typeid(T).name() << std::endl;
  //#error not implemented yet: typeid(T).name()
  TEST_ASSERT_FALSE(true);
}

/// @brief Specialization for T = bool
/// @param actual current value
/// @param expected expected value
template<>
void EXPECT_EQ<bool, bool>(bool actual, bool expected)
{
  if (expected)
  {
    TEST_ASSERT_TRUE(actual);
  }
  else
  {
    TEST_ASSERT_FALSE(actual);
  }
}

// Remark: [C++11 standard, 3.9.1 Fundamental types] "Plain char, signed char, and unsigned char are three distinct types."

template<> void EXPECT_EQ<char, char>(char actual, char expected)        { TEST_ASSERT_EQUAL_CHAR(expected, actual); }
template<> void EXPECT_EQ<sint8,sint8>(sint8 actual, sint8 expected)     { TEST_ASSERT_EQUAL_INT8(expected, actual); }
template<> void EXPECT_EQ<sint16,sint16>(sint16 actual, sint16 expected)  { TEST_ASSERT_EQUAL_INT16(expected, actual); }
template<> void EXPECT_EQ<sint32,sint32>(sint32 actual, sint32 expected)  { TEST_ASSERT_EQUAL_INT32(expected, actual); }
template<> void EXPECT_EQ<sint64,sint64>(sint64 actual, sint64 expected)  { TEST_ASSERT_EQUAL_INT64(expected, actual); }
template<> void EXPECT_EQ<uint8,uint8>(uint8 actual, uint8 expected)     { TEST_ASSERT_EQUAL_UINT8(expected, actual); }
template<> void EXPECT_EQ<uint16,uint16>(uint16 actual, uint16 expected)  { TEST_ASSERT_EQUAL_UINT16(expected, actual); }
template<> void EXPECT_EQ<uint32,uint32>(uint32 actual, uint32 expected)  { TEST_ASSERT_EQUAL_UINT32(expected, actual); }
template<> void EXPECT_EQ<uint64,uint64>(uint64 actual, uint64 expected)  { TEST_ASSERT_EQUAL_UINT64(expected, actual); }
template<> void EXPECT_EQ<uint64,uint32>(uint64 actual, uint32 expected)  { TEST_ASSERT_EQUAL_UINT64(static_cast<uint64>(expected), actual); }
template<> void EXPECT_EQ<util::streampos>(util::streampos actual, util::streampos expected) { EXPECT_EQ<util::streamoff>(expected, actual); }

/// Declares a test function
#define TEST(group, test_fct)       void test_fct(void)

#endif // UNITY_ADAPT_H
