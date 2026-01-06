/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/algorithm.h
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Util/Algorithm.h>
#include <unity_adapt.h>

TEST(Ut_Algorithm, is_in_interval)
{
}


void setUp(void)
{
}

void tearDown(void)
{
}

void test_setup(void)
{
}

bool test_loop(void)
{
  UNITY_BEGIN();

  RUN_TEST(is_in_interval);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
