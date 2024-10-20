/**
 * @file Ut_Vector/Test.cpp
 *
 * @brief Unit tests for class Vector of Gen/Util/Vector.h
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
 * See <https://www.gnu.org/licenses/>.
 */

#include <unity_adapt.h>
#include <Util/Vector.h>
#include <Test/Logger.h>
#include <vector>

// ------------------------------------------------------------------------------------------------
/// Helper class to count 
// ------------------------------------------------------------------------------------------------
class A
{
  public:
  
  int data;

  static int cnt_construct_default;
  static int cnt_construct_int;
  static int cnt_construct_copy;
  static int cnt_construct_move;
  static int cnt_assignment_copy;
  static int cnt_assignment_move;

  A() : data(0)
  {
    cnt_construct_default++;
  }
  A(int d) : data(d)
  {
    cnt_construct_int++;
  }
  A(const A& a) : data(a.data)
  {
    cnt_construct_copy++;
  }
  A(const A&& a) : data(a.data)
  {
    cnt_construct_move++;
  }
  A& operator=(const A& a)
  {
    cnt_assignment_copy++;
    data = a.data;
    return *this;
  }
  A& operator=(const A&& a)
  {
    cnt_assignment_move++;
    data = a.data;
    return *this;
  }
};

int A::cnt_construct_default = 0;
int A::cnt_construct_int = 0;
int A::cnt_construct_copy = 0;
int A::cnt_construct_move = 0;
int A::cnt_assignment_copy = 0;
int A::cnt_assignment_move = 0;

static void initA()
{
  A::cnt_construct_default = 0;
  A::cnt_construct_int = 0;
  A::cnt_construct_copy = 0;
  A::cnt_construct_move = 0;
  A::cnt_assignment_copy = 0;
  A::cnt_assignment_move = 0;
}

// ------------------------------------------------------------------------------------------------
/// Default constructor
// ------------------------------------------------------------------------------------------------
TEST(Ut_Vector, Construct_default)
{
  using vector_type = util::vector<int>;
  
  vector_type vec;

  EXPECT_EQ(vec.size(), 0U);
}

// ------------------------------------------------------------------------------------------------
/// push_back
// ------------------------------------------------------------------------------------------------
TEST(Ut_Vector, Push_back_lvalue)
{
  using vector_type = util::vector<A>;
  
  constexpr int N = 16; // default number of elements in the vector

  vector_type vec;
  A a;

  EXPECT_EQ(vec.size(), 0U);
  EXPECT_EQ(A::cnt_construct_default, N+1);

  a.data = 1;
  vec.push_back(a);
  EXPECT_EQ(vec.size(), 1U);
  EXPECT_EQ(A::cnt_construct_default, N+1);
  EXPECT_EQ(A::cnt_construct_int    , 0);
  EXPECT_EQ(A::cnt_construct_copy   , 0);
  EXPECT_EQ(A::cnt_construct_move   , 0);
  EXPECT_EQ(A::cnt_assignment_copy  , 1);
  EXPECT_EQ(A::cnt_assignment_move  , 0);
  EXPECT_EQ(vec.back().data, a.data);
}

// ------------------------------------------------------------------------------------------------
/// push_back rvalue
// ------------------------------------------------------------------------------------------------
TEST(Ut_Vector, Push_back_rvalue)
{
  using vector_type = util::vector<A>;
  
  constexpr int N = 16; // default number of elements in the vector

  vector_type vec;
  A a;

  EXPECT_EQ(vec.size(), 0U);
  EXPECT_EQ(A::cnt_construct_default, N+1);

  vec.push_back(A());
  EXPECT_EQ(vec.size(), 1U);
  EXPECT_EQ(A::cnt_construct_default, N+2);
  EXPECT_EQ(A::cnt_construct_int    , 0);
  EXPECT_EQ(A::cnt_construct_copy   , 0);
  EXPECT_EQ(A::cnt_construct_move   , 0);
  EXPECT_EQ(A::cnt_assignment_copy  , 0);
  EXPECT_EQ(A::cnt_assignment_move  , 1);
  EXPECT_EQ(vec.back().data, a.data);
}

// ------------------------------------------------------------------------------------------------
/// emplace_back
// ------------------------------------------------------------------------------------------------
TEST(Ut_Vector, emplace_back_lvalue)
{
  using vector_type = util::vector<A>;
  
  constexpr int N = 16; // default number of elements in the vector

  vector_type vec;
  A a;

  EXPECT_EQ(vec.size(), 0U);
  EXPECT_EQ(A::cnt_construct_default, N+1);

  a.data = 1;
  vec.emplace_back(a);
  EXPECT_EQ(vec.size(), 1U);
  EXPECT_EQ(A::cnt_construct_default, N+1);
  EXPECT_EQ(A::cnt_construct_int    , 0);
  EXPECT_EQ(A::cnt_construct_copy   , 1);
  EXPECT_EQ(A::cnt_construct_move   , 0);
  EXPECT_EQ(A::cnt_assignment_copy  , 0);
  EXPECT_EQ(A::cnt_assignment_move  , 0);
  EXPECT_EQ(vec.back().data, a.data);

  initA();
  std::vector<A> stdvec;
  stdvec.emplace_back(a);
  EXPECT_EQ(A::cnt_construct_copy, 1);
  EXPECT_EQ(A::cnt_assignment_copy, 0);
  EXPECT_EQ(stdvec.back().data, a.data);
}

// ------------------------------------------------------------------------------------------------
/// emplace_back int
// ------------------------------------------------------------------------------------------------
TEST(Ut_Vector, emplace_back_int)
{
  using vector_type = util::vector<A>;
  
  constexpr int N = 16; // default number of elements in the vector

  vector_type vec;
  int val;

  EXPECT_EQ(vec.size(), 0U);
  EXPECT_EQ(A::cnt_construct_default, N);

  val = 1;
  vec.emplace_back(val);
  EXPECT_EQ(vec.size(), 1U);
  EXPECT_EQ(A::cnt_construct_default, N);
  EXPECT_EQ(A::cnt_construct_int    , 1);
  EXPECT_EQ(A::cnt_construct_copy   , 0);
  EXPECT_EQ(A::cnt_construct_move   , 0);
  EXPECT_EQ(A::cnt_assignment_copy  , 0);
  EXPECT_EQ(A::cnt_assignment_move  , 0);
  EXPECT_EQ(vec.back().data, val);

  initA();
  std::vector<A> stdvec;
  stdvec.emplace_back(val);
  EXPECT_EQ(A::cnt_construct_default, 0);
  EXPECT_EQ(A::cnt_construct_int    , 1);
  EXPECT_EQ(A::cnt_construct_copy   , 0);
  EXPECT_EQ(A::cnt_construct_move   , 0);
  EXPECT_EQ(A::cnt_assignment_copy  , 0);
  EXPECT_EQ(A::cnt_assignment_move  , 0);
  EXPECT_EQ(stdvec.back().data, val);
}

void setUp(void)
{
  initA();
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

  RUN_TEST(Construct_default);
  RUN_TEST(Push_back_lvalue);
  RUN_TEST(Push_back_rvalue);
  RUN_TEST(emplace_back_lvalue);
  RUN_TEST(emplace_back_int);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
