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

#include <Unity.h>
#include <Std_Types.h>

/// @brief Introduce Google Test like expect macros
/// @tparam T Type to be checked such as uint8, int, size_t
/// @param actual current value
/// @param expected expected value
template<typename T>
void EXPECT_EQ(T actual, T expected)
{
  // not implemented yet
  TEST_ASSERT_FALSE(true);
}

/// @brief Specialization for T = bool
/// @param actual current value
/// @param expected expected value
template<>
void EXPECT_EQ<bool>(bool actual, bool expected)
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

template<> void EXPECT_EQ<int>(int actual, int expected)          { TEST_ASSERT_EQUAL_INT(expected, actual); }
template<> void EXPECT_EQ<uint8>(uint8 actual, uint8 expected)    { TEST_ASSERT_EQUAL_UINT8(expected, actual); }
template<> void EXPECT_EQ<uint16>(uint16 actual, uint16 expected) { TEST_ASSERT_EQUAL_UINT16(expected, actual); }
template<> void EXPECT_EQ<size_t>(size_t actual, size_t expected) { TEST_ASSERT_EQUAL_size_t(expected, actual); }

/// Declares a test function
#define TEST(group, test_fct)       void test_fct(void)

#endif // UNITY_ADAPT_H
