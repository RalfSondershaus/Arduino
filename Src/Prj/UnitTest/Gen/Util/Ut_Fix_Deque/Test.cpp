/**
  * @file Test.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief Test for Gen/Util/Fix_Deque.h
  *
  * @copyright Copyright 2023 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <Util/Fix_Deque.h>
#include <unity_adapt.h>

  /// Construtor with deque size 5
TEST(Ut_Fix_Deque, construct_1)
{
  using deque_type = util::fix_deque<int, 5>;
  deque_type mydeque;

  EXPECT_EQ(mydeque.size(), deque_type::size_type{ 0 });
  EXPECT_EQ(mydeque.empty(), true);
}

/// Test push back
/// 
/// - push_back until full
/// - push_back another element afterwards to check if overflow protection works
/// - size()
/// - back()
/// - begin(), end(), iterator value
/// 
/// push_back with  { 1, 2, 3, 4, 5, 6 }
/// expected result { 1, 2, 3, 4, 5 }
TEST(Ut_Fix_Deque, push_back_1)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  int i;
  deque_type::size_type size;

  size = 0;
  for (i = 0; i < N; i++)
  {
    n = i + 1;
    size++;
    mydeque.push_back(n);
    EXPECT_EQ(mydeque.size(), size);
    EXPECT_EQ(mydeque.back(), n);
  }
  n = i + 1;
  mydeque.push_back(n);
  EXPECT_EQ(mydeque.size(), static_cast<deque_type::size_type>(N));
  EXPECT_EQ(mydeque.back(), N);

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
    EXPECT_EQ(*cit, n);
  }
}

/// Test push_front
/// 
/// - push_front until full
/// - push_front another element afterwards to check if overflow protection works
/// - size()
/// - back()
/// - begin(), end(), iterator value
/// 
/// push_front with { 1, 2, 3, 4, 5, 6 }
/// expected result { 5, 4, 3, 2, 1 }
TEST(Ut_Fix_Deque, push_front_1)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  int i;
  deque_type::size_type size;

  size = 0;
  for (i = 0; i < N; i++)
  {
    n = i + 1;
    size++;
    mydeque.push_front(n);
    EXPECT_EQ(mydeque.size(), size);
    EXPECT_EQ(mydeque.front(), n);
  }
  n = i + 1;
  mydeque.push_front(n);
  EXPECT_EQ(mydeque.size(), static_cast<deque_type::size_type>(N));
  EXPECT_EQ(mydeque.front(), N);

  n = 6;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n--;
    EXPECT_EQ(*cit, n);
  }
}

/// Test mixture of push_front and push_back
/// 
/// - push_front and push_back until full
/// - size()
/// - back()
/// - begin(), end(), iterator value
/// 
/// push_front 1  { 1 }
/// push_back 2   { 1, 2 }
/// push_front 3  { 3, 1, 2 }
/// push_front 4  { 4, 3, 1, 2 }
/// push_back 5  { 4, 3, 1, 2, 5 }
TEST(Ut_Fix_Deque, push_front_push_back_1)
{
  static const int anExp1[] = { 1 };
  static const int anExp2[] = { 1, 2 };
  static const int anExp3[] = { 3, 1, 2 };
  static const int anExp4[] = { 4, 3, 1, 2 };
  static const int anExp5[] = { 4, 3, 1, 2, 5 };
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  deque_type::size_type size;
  deque_type::size_type pos;

  size = 0;

  n = 1;
  size++;
  mydeque.push_front(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.front(), n);
  pos = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    EXPECT_EQ(*cit, anExp1[pos]);
    pos++;
  }

  n = 2;
  size++;
  mydeque.push_back(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.back(), n);
  pos = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    EXPECT_EQ(*cit, anExp2[pos]);
    pos++;
  }

  n = 3;
  size++;
  mydeque.push_front(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.front(), n);
  pos = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    EXPECT_EQ(*cit, anExp3[pos]);
    pos++;
  }

  n = 4;
  size++;
  mydeque.push_front(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.front(), n);
  pos = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    EXPECT_EQ(*cit, anExp4[pos]);
    pos++;
  }

  n = 5;
  size++;
  mydeque.push_back(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.back(), n);

  size = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    EXPECT_EQ(*cit, anExp5[size]);
    size++;
  }
}

/// Test pop_back for a full container
/// 
/// - push_back until full
/// - push_back another element afterwards to check if overflow protection works
/// - size()
/// - back()
/// - empty()
/// - begin(), end(), iterator value
/// 
/// Two runs:
/// 
/// First run
/// 
/// push_back with  { 1, 2, 3, 4, 5 }
/// expected result { 1, 2, 3, 4, 5 }
/// pop_back
/// { 1, 2, 3, 4 }
/// { 1, 2, 3 }
/// { 1, 2 }
/// { 1 }
/// empty
/// 
/// Second run
/// 
/// push_back with  { 1, 2, 3, 4, 5 }
/// expected result { 1, 2, 3, 4, 5 }
/// pop_back
/// { 1, 2, 3, 4 }
/// { 1, 2, 3 }
/// { 1, 2 }
/// { 1 }
/// empty
TEST(Ut_Fix_Deque, pop_back_1)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  int i;
  deque_type::size_type size;

  // first run
  size = 0;
  for (i = 0; i < N; i++)
  {
    n = i + 1;
    size++;
    mydeque.push_back(n);
    EXPECT_EQ(mydeque.size(), size);
    EXPECT_EQ(mydeque.back(), n);
  }

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
    EXPECT_EQ(*cit, n);
  }

  n = 5;
  size = mydeque.size();
  for (i = 0; i < N; i++)
  {
    mydeque.pop_back();
    size--;
    n--;
    EXPECT_EQ(mydeque.size(), size);
    if (size > 0U)
    {
      // Calling back on an empty container causes undefined behavior.
      EXPECT_EQ(mydeque.back(), n);
    }
  }

  EXPECT_EQ(mydeque.empty(), (bool)true);

  // second run
  size = 0;
  for (i = 0; i < N; i++)
  {
    n = i + 1;
    size++;
    mydeque.push_back(n);
    EXPECT_EQ(mydeque.size(), size);
    EXPECT_EQ(mydeque.back(), n);
  }

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
    EXPECT_EQ(*cit, n);
  }

  n = 5;
  size = mydeque.size();
  for (i = 0; i < N; i++)
  {
    mydeque.pop_back();
    size--;
    n--;
    EXPECT_EQ(mydeque.size(), size);
    if (size > 0U)
    {
      // Calling back on an empty container causes undefined behavior.
      EXPECT_EQ(mydeque.back(), n);
    }
  }

  EXPECT_EQ(mydeque.empty(), (bool)true);
}

/// Test pop_front for a full container
/// 
/// - push_back until full
/// - size()
/// - back()
/// - front()
/// - empty()
/// - begin(), end(), iterator value
/// 
/// Two runs:
/// 
/// First run
/// 
/// push_back with  { 1, 2, 3, 4, 5 }
/// expected result { 1, 2, 3, 4, 5 }
/// pop_front
/// { 2, 3, 4, 5 }
/// { 3, 4, 5 }
/// { 4, 5 }
/// { 5 }
/// empty
/// 
/// Second run
/// 
/// push_back with  { 1, 2, 3, 4, 5 }
/// expected result { 1, 2, 3, 4, 5 }
/// pop_front
/// { 2, 3, 4, 5 }
/// { 3, 4, 5 }
/// { 4, 5 }
/// { 5 }
/// empty
TEST(Ut_Fix_Deque, pop_front_1)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  int i;
  deque_type::size_type size;

  // first run
  size = 0;
  for (i = 0; i < N; i++)
  {
    n = i + 1;
    size++;
    mydeque.push_back(n);
    EXPECT_EQ(mydeque.size(), size);
    EXPECT_EQ(mydeque.back(), n);
  }

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
    EXPECT_EQ(*cit, n);
  }

  n = 1;
  size = mydeque.size();
  for (i = 0; i < N; i++)
  {
    mydeque.pop_front();
    size--;
    n++;
    EXPECT_EQ(mydeque.size(), size);
    if (size > 0U)
    {
      // Calling back on an empty container causes undefined behavior.
      EXPECT_EQ(mydeque.front(), n);
    }
  }

  EXPECT_EQ(mydeque.empty(), (bool)true);

  // second run
  size = 0;
  for (i = 0; i < N; i++)
  {
    n = i + 1;
    size++;
    mydeque.push_back(n);
    EXPECT_EQ(mydeque.size(), size);
    EXPECT_EQ(mydeque.back(), n);
  }

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
    EXPECT_EQ(*cit, n);
  }

  n = 1;
  size = mydeque.size();
  for (i = 0; i < N; i++)
  {
    mydeque.pop_front();
    size--;
    n++;
    EXPECT_EQ(mydeque.size(), size);
    if (size > 0U)
    {
      // Calling back on an empty container causes undefined behavior.
      EXPECT_EQ(mydeque.front(), n);
    }
  }

  EXPECT_EQ(mydeque.empty(), (bool)true);
}

/// Test pop_front for a container with one element
/// 
/// - push_back of one element
/// - size()
/// - back()
/// - front()
/// - empty()
/// - begin(), end(), iterator value
/// 
/// push_back with  { 1 }
/// expected result { 1 }
/// pop_front
/// expected result { }, empty
TEST(Ut_Fix_Deque, pop_front_2)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  deque_type::size_type size;

  EXPECT_EQ(mydeque.empty(), true);

  size = 1U;
  n = 1;
  mydeque.push_back(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.back(), n);
  EXPECT_EQ(mydeque.front(), n);
  EXPECT_EQ(mydeque.empty(), false);

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
    EXPECT_EQ(*cit, n);
  }
  // check if loop was executed once
  EXPECT_EQ(n, 1);

  size = 0U;
  mydeque.pop_front();
  EXPECT_EQ(mydeque.size(), size);

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
  }
  EXPECT_EQ(n, 0);
}

/// Test pop_back for a container with one element
/// 
/// - push_back of one element
/// - size()
/// - back()
/// - front()
/// - empty()
/// - begin(), end(), iterator value
/// 
/// push_back with  { 1 }
/// expected result { 1 }
/// pop_back
/// expected result { }, empty
TEST(Ut_Fix_Deque, pop_back_2)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  deque_type::size_type size;

  size = 1U;
  n = 1;
  mydeque.push_back(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.back(), n);
  EXPECT_EQ(mydeque.front(), n);
  EXPECT_EQ(mydeque.empty(), false);

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
    EXPECT_EQ(*cit, n);
  }
  // check if loop was executed once
  EXPECT_EQ(n, 1);

  size = 0U;
  mydeque.pop_back();
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.empty(), true);

  n = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    n++;
  }
  EXPECT_EQ(n, 0);
}

/// Test mixture of push_back, push_front, pop_back and pop_front 
/// with wrap around at end of buffer.
/// 
/// - push_back(), push_front()
/// - pop_back(), pop_front()
/// - size()
/// - empty()
/// - begin(), end(), iterator value
/// 
/// See table of test steps
/// 
TEST(Ut_Fix_Deque, push_front_push_back_2)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;

  typedef enum { PUSH_BACK = 0, PUSH_FRONT = 1, POP_BACK = 2, POP_FRONT = 3 } tCmd;

  typedef struct
  {
    tCmd cmd;
    int nElement;
    struct
    {
      deque_type::size_type unSize;
      int anElements[N];
    } expected;
  } tTestStep;

  static const tTestStep aMyTestSteps[] =
  {
    { PUSH_BACK , 1, { 1U, { 1, 0, 0, 0, 0 } } },
    { PUSH_BACK , 2, { 2U, { 1, 2, 0, 0, 0 } } },
    { PUSH_BACK , 3, { 3U, { 1, 2, 3, 0, 0 } } },
    { PUSH_BACK , 4, { 4U, { 1, 2, 3, 4, 0 } } },
    { PUSH_FRONT, 5, { 5U, { 5, 1, 2, 3, 4 } } },
    { POP_BACK  , 0, { 4U, { 5, 1, 2, 3, 0 } } },
    { PUSH_FRONT, 6, { 5U, { 6, 5, 1, 2, 3 } } },
    { POP_BACK  , 0, { 4U, { 6, 5, 1, 2, 0 } } },
    { PUSH_BACK , 7, { 5U, { 6, 5, 1, 2, 7 } } },
    { POP_FRONT , 0, { 4U, { 5, 1, 2, 7, 0 } } },
    { PUSH_FRONT, 8, { 5U, { 8, 5, 1, 2, 7 } } },
    { POP_FRONT , 0, { 4U, { 5, 1, 2, 7, 0 } } },
    { POP_FRONT , 0, { 3U, { 1, 2, 7, 0, 0 } } },
    { POP_BACK  , 0, { 2U, { 1, 2, 0, 0, 0 } } },
    { POP_FRONT , 0, { 1U, { 2, 0, 0, 0, 0 } } },
    { POP_FRONT , 0, { 0U, { 0, 0, 0, 0, 0 } } },
    { PUSH_FRONT, 1, { 1U, { 1, 0, 0, 0, 0 } } }
  };
  deque_type mydeque;
  size_t i;
  int n;

  for (i = 0; i < sizeof(aMyTestSteps) / sizeof(tTestStep); i++)
  {
    const tTestStep& ts = aMyTestSteps[i];
    switch (ts.cmd)
    {
    case PUSH_BACK: { mydeque.push_back(ts.nElement); } break;
    case PUSH_FRONT: { mydeque.push_front(ts.nElement); } break;
    case POP_FRONT: { mydeque.pop_front(); } break;
    case POP_BACK: { mydeque.pop_back(); } break;
    default: break;
    }
    EXPECT_EQ(mydeque.size(), ts.expected.unSize);
    if (ts.expected.unSize == 0U)
    {
      EXPECT_EQ(mydeque.empty(), true);
    }
    else
    {
      EXPECT_EQ(mydeque.empty(), false);
    }
    n = 0;
    for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
    {
      EXPECT_EQ(*cit, ts.expected.anElements[n]);
      n++;
    }
  }
}

template<class Deque>
typename Deque::value_type calc_sum(const Deque& mydeque)
{
  typename Deque::value_type sum = 0;
  for (auto cit = mydeque.begin(); cit != mydeque.end(); cit++)
  {
    sum += *cit;
  }
  return sum;
}

/// Test const_iterator
/// 
/// - push_back of one element
/// - size()
/// - back()
/// - front()
/// - begin(), end(), const iterator value
TEST(Ut_Fix_Deque, const_iterator_1)
{
  constexpr int N = 5;
  using deque_type = util::fix_deque<int, N>;
  deque_type mydeque;
  int n;
  deque_type::size_type size;

  size = 1U;
  n = 1;
  mydeque.push_back(n);
  EXPECT_EQ(mydeque.size(), size);
  EXPECT_EQ(mydeque.back(), n);
  EXPECT_EQ(mydeque.front(), n);

  n = calc_sum(mydeque);

  EXPECT_EQ(n, 1);
}

TEST(Ut_Fix_Deque_Bool, test_fix_deque_bool_contstruct_1)
{
  constexpr size_t kMaxSize = 400;
  using deque_t = util::fix_deque<bool, kMaxSize>;
  deque_t myDeque;
  EXPECT_EQ(myDeque.empty(), true);
  EXPECT_EQ(myDeque.size(), static_cast<size_t>(0));
}

TEST(Ut_Fix_Deque_Bool, test_fix_deque_bool_basic)
{
  using util::fix_deque_bool;
  fix_deque_bool<5> dq;

  // Test empty
  EXPECT_EQ(dq.empty(), true);
  EXPECT_EQ(dq.size() == 0, true);

  // Test push_back
  dq.push_back(true);
  EXPECT_EQ(!dq.empty(), true);
  EXPECT_EQ(dq.size() == 1, true);
  EXPECT_EQ(dq.front() == true, true);
  EXPECT_EQ(dq.back() == true, true);

  dq.push_back(false);
  EXPECT_EQ(dq.size() == 2, true);
  EXPECT_EQ(dq.front() == true, true);
  EXPECT_EQ(dq.back() == false, true);

  dq.push_back(true);
  dq.push_back(false);
  dq.push_back(true);
  EXPECT_EQ(dq.size() == 5, true);
  EXPECT_EQ(dq.front() == true, true);
  EXPECT_EQ(dq.back() == true, true);

  // Test max_size
  EXPECT_EQ(dq.max_size() == 5, true);

  // Test at and operator[]
  EXPECT_EQ(dq.at(0) == true, true);
  EXPECT_EQ(dq.at(1) == false, true);
  EXPECT_EQ(dq[2] == true, true);
  EXPECT_EQ(dq[3] == false, true);
  EXPECT_EQ(dq[4] == true, true);

  // Test push_back when full (should not add)
  dq.push_back(false);
  EXPECT_EQ(dq.size() == 5, true);

  // Test pop_front
  dq.pop_front();
  EXPECT_EQ(dq.size() == 4, true);
  EXPECT_EQ(dq.front() == false, true);

  dq.pop_front();
  dq.pop_front();
  dq.pop_front();
  dq.pop_front();
  EXPECT_EQ(dq.empty(), true);

  // Test clear
  dq.push_back(true);
  dq.push_back(false);
  dq.clear();
  EXPECT_EQ(dq.empty(), true);
  EXPECT_EQ(dq.size() == 0, true);
}

TEST(Ut_Fix_Deque_Bool, test_fix_deque_bool_wraparound)
{
  using util::fix_deque_bool;
  fix_deque_bool<3> dq;

  dq.push_back(true);
  dq.push_back(false);
  dq.push_back(true);

  // Remove two elements, then add two more to test wraparound
  dq.pop_front();
  dq.pop_front();
  dq.push_back(false);
  dq.push_back(true);

  EXPECT_TRUE(dq.size() == 3);
  EXPECT_TRUE(dq.front() == true);
  EXPECT_TRUE(dq.back() == true);
  EXPECT_TRUE(dq.at(0) == true);
  EXPECT_TRUE(dq.at(1) == false);
  EXPECT_TRUE(dq.at(2) == true);
}

TEST(Ut_Fix_Deque_Bool, test_fix_deque_bool_full_empty_cycle)
{
  using util::fix_deque_bool;
  fix_deque_bool<2> dq;

  // Fill and empty multiple times
  for (int i = 0; i < 10; ++i) {
      dq.clear();
      EXPECT_TRUE(dq.empty());
      dq.push_back(true);
      dq.push_back(false);
      EXPECT_TRUE(dq.size() == 2);
      EXPECT_TRUE(dq.front() == true);
      EXPECT_TRUE(dq.back() == false);
      dq.pop_front();
      dq.pop_front();
      EXPECT_TRUE(dq.empty());
  }
}

TEST(Ut_Fix_Deque_Bool, test_fix_deque_bool_edge_cases)
{
  using util::fix_deque_bool;
  fix_deque_bool<1> dq;

  EXPECT_TRUE(dq.empty());
  dq.push_back(true);
  EXPECT_TRUE(dq.size() == 1);
  EXPECT_TRUE(dq.front() == true);
  dq.pop_front();
  EXPECT_TRUE(dq.empty());

  // Try popping from empty deque (should not crash)
  dq.pop_front();
  EXPECT_TRUE(dq.empty());

  // Try pushing more than max_size
  dq.push_back(false);
  dq.push_back(true);
  EXPECT_TRUE(dq.size() == 1);
  EXPECT_TRUE(dq.front() == false);
}

/// @brief Test fix_deque_bool with 400 elements.
/// 
/// This test fills a fix_deque_bool<400> with alternating true/false values,
/// checks correct storage and retrieval, tests wraparound by removing and adding elements,
/// and verifies that clear() resets the container.
TEST(Ut_Fix_Deque_Bool, test_fix_deque_bool_400)
{
    constexpr size_t N = 400;
    using util::fix_deque_bool;
    fix_deque_bool<N> dq;

    // Initially empty
    EXPECT_TRUE(dq.empty());
    EXPECT_TRUE(dq.size() == 0);

    // Fill with alternating true/false
    for (size_t i = 0; i < N; ++i) {
        dq.push_back(i % 2 == 0);
        EXPECT_TRUE(dq.size() == i + 1);
        EXPECT_TRUE(dq.back() == (i % 2 == 0));
    }
    EXPECT_TRUE(dq.size() == N);
    EXPECT_TRUE(!dq.empty());

    // Check all values
    for (size_t i = 0; i < N; ++i) {
        EXPECT_TRUE(dq.at(i) == (i % 2 == 0));
        EXPECT_TRUE(dq[i] == (i % 2 == 0));
    }

    // Remove half, check front
    for (size_t i = 0; i < N / 2; ++i) {
        EXPECT_TRUE(dq.front() == (i % 2 == 0));
        dq.pop_front();
        EXPECT_TRUE(dq.size() == N - i - 1);
    }

    // Add more to wrap around
    for (size_t i = 0; i < N / 2; ++i) {
        dq.push_back(true);
        EXPECT_TRUE(dq.back() == true);
    }
    EXPECT_TRUE(dq.size() == N);

    // Clear and check
    dq.clear();
    EXPECT_TRUE(dq.empty());
    EXPECT_TRUE(dq.size() == 0);
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

  RUN_TEST(construct_1);
  RUN_TEST(push_back_1);
  RUN_TEST(push_front_1);
  RUN_TEST(push_front_push_back_1);
  RUN_TEST(pop_back_1);
  RUN_TEST(pop_front_1);
  RUN_TEST(pop_front_2);
  RUN_TEST(pop_back_2);
  RUN_TEST(push_front_push_back_2);
  RUN_TEST(const_iterator_1);

  RUN_TEST(test_fix_deque_bool_contstruct_1);
  RUN_TEST(test_fix_deque_bool_basic);
  RUN_TEST(test_fix_deque_bool_wraparound);
  RUN_TEST(test_fix_deque_bool_full_empty_cycle);
  RUN_TEST(test_fix_deque_bool_edge_cases);
  RUN_TEST(test_fix_deque_bool_400);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
