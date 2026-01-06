/**
  * @file Test.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief Test for Gen/Util/Fix_Queue.h
  *
  * @copyright Copyright 2023 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <Util/Fix_Queue.h>
#include <unity_adapt.h>

  /// Construtor with queue size 5
TEST(Ut_Fix_Queue, construct_1)
{
  using queue_type = util::fix_queue<int, 5>;
  queue_type myqueue;

  EXPECT_EQ(myqueue.size(), queue_type::size_type{ 0 });
  EXPECT_EQ(myqueue.empty(), true);
}

/// Test push and pop with maximal number of elements
/// 
/// - push until full
/// - push another element afterwards to check if overflow protection works
/// - pop all elements
/// 
/// - size()
/// - push()
/// - pop()
/// - front()
/// - back()
/// 
/// push with       { 1, 2, 3, 4, 5, 6 }
/// expected result { 1, 2, 3, 4, 5 }
/// 
TEST(Ut_Fix_Queue, push_pop_1)
{
  constexpr int N = 5;
  using queue_type = util::fix_queue<int, N>;
  queue_type myfifo;
  int n;
  int i;
  queue_type::size_type size;

  size = 0;
  for (i = 0; i < N; i++)
  {
    n = i + 1;
    size++;
    myfifo.push(n);
    EXPECT_EQ(myfifo.size(), size);
    EXPECT_EQ(myfifo.back(), n);
    EXPECT_EQ(myfifo.front(), 1);
  }
  n = i + 1;
  myfifo.push(n);
  EXPECT_EQ(myfifo.size(), static_cast<queue_type::size_type>(N));
  EXPECT_EQ(myfifo.back(), N);
  EXPECT_EQ(myfifo.front(), 1);

  for (i = 0; i < N; i++)
  {
    n = i + 1;
    EXPECT_EQ(myfifo.front(), n);
    EXPECT_EQ(myfifo.size(), size);
    myfifo.pop();
    size--;
    EXPECT_EQ(myfifo.size(), size);
    if (size == 0)
    {
      EXPECT_EQ(myfifo.empty(), true);
    }
    else
    {
      EXPECT_EQ(myfifo.empty(), false);
    }
  }
}

/// Test push and pop with one element
/// 
/// - push one element
/// - pop element
/// 
/// - size()
/// - push()
/// - pop()
/// - front()
/// - back()
/// 
/// push with       { 1 }
/// expected result { 1 }
/// 
TEST(Ut_Fix_Queue, push_pop_2)
{
  constexpr int N = 5;
  using queue_type = util::fix_queue<int, N>;
  queue_type myfifo;
  queue_type::size_type size;

  size = 1U;
  myfifo.push(1);
  EXPECT_EQ(myfifo.size(), size);
  EXPECT_EQ(myfifo.back(), 1);
  EXPECT_EQ(myfifo.front(), 1);
  myfifo.pop();
  size = 0U;
  EXPECT_EQ(myfifo.size(), size);
  EXPECT_EQ(myfifo.empty(), true);
}

/// Test push and pop with elements and an empty FIFO inbetween
/// 
TEST(Ut_Fix_Queue, push_pop_3)
{
  constexpr int N = 200;
  using queue_type = util::fix_queue<int, N>;
  queue_type myfifo;
  constexpr int CNT = 3*N/4;
  size_t cnt = 0;

  for (int i = 0; i < CNT; i++)
  {
    EXPECT_EQ(myfifo.size(), cnt);
    myfifo.push(i);
    cnt++;
    EXPECT_EQ(myfifo.size(), cnt);
    EXPECT_EQ(myfifo.back(), i);
    EXPECT_EQ(myfifo.front(), 0); // should be first number pushed
  }
  for (int i = 0; i < CNT; i++)
  {
    EXPECT_EQ(myfifo.size(), cnt);
    int fi = myfifo.front();
    myfifo.pop();
    cnt--;
    EXPECT_EQ(myfifo.size(), cnt);
    EXPECT_EQ(fi, i);
  }
  cnt = 0;
  for (int i = 1; i < CNT; i++)
  {
    EXPECT_EQ(myfifo.size(), cnt);
    myfifo.push(i);
    cnt++;
    EXPECT_EQ(myfifo.size(), cnt);
    EXPECT_EQ(myfifo.back(), i);
    EXPECT_EQ(myfifo.front(), 1); // should be first number pushed
  }
  for (int i = 1; i < CNT; i++)
  {
    EXPECT_EQ(myfifo.size(), cnt);
    int fi = myfifo.front();
    myfifo.pop();
    cnt--;
    EXPECT_EQ(myfifo.size(), cnt);
    EXPECT_EQ(fi, i);
  }
}

using util::fix_queue_bool;

/// @brief Tests basic queue operations: push, front, back, pop, empty, and size.
/// @details Test Design: Functional/Black-box. Verifies FIFO behavior and correct size tracking.
TEST(FixQueueBool, BasicOperations)
{
    fix_queue_bool<5> q;

    EXPECT_TRUE(q.empty());
    EXPECT_TRUE(q.size() == 0);

    q.push(true);
    EXPECT_FALSE(q.empty());
    EXPECT_TRUE(q.size() == 1);
    EXPECT_TRUE(q.front());
    EXPECT_TRUE(q.back());

    q.push(false);
    EXPECT_TRUE(q.size() == 2);
    EXPECT_TRUE(q.front());
    EXPECT_FALSE(q.back());

    q.pop();
    EXPECT_TRUE(q.size() == 1);
    EXPECT_FALSE(q.front());
    EXPECT_FALSE(q.back());

    q.pop();
    EXPECT_TRUE(q.empty());
    EXPECT_TRUE(q.size() == 0);
}

/// @brief Tests queue capacity and overflow behavior.
/// @details Test Design: Boundary Value Analysis. Ensures queue does not exceed its fixed size.
TEST(FixQueueBool, Capacity)
{
    constexpr size_t N = 3;
    fix_queue_bool<N> q;

    for (size_t i = 0; i < N; ++i) {
        q.push(i % 2 == 0);
        EXPECT_TRUE(q.size() == i + 1);
    }
    // Try to push beyond capacity
    q.push(true);
    EXPECT_TRUE(q.size() == N); // Should not grow beyond N

    // Check values
    EXPECT_TRUE(q.front());
    EXPECT_TRUE(q.back());
}

/// @brief Tests wraparound behavior after popping and pushing elements.
/// @details Test Design: State Transition/Edge Case. Ensures correct circular buffer logic.
TEST(FixQueueBool, Wraparound)
{
    fix_queue_bool<4> q;

    q.push(true);
    q.push(false);
    q.push(true);
    q.push(false);

    q.pop();
    q.pop();

    q.push(true);
    q.push(false);

    EXPECT_TRUE(q.size() == 4);
    EXPECT_TRUE(q.front());
    EXPECT_FALSE(q.back());
}

/// @brief Tests queue with a large number of elements (400).
/// @details Test Design: Stress/Scalability. Verifies correct operation with large N.
TEST(FixQueueBool, LargeQueue)
{
    constexpr size_t N = 400;
    fix_queue_bool<N> q;

    for (size_t i = 0; i < N; ++i) {
        q.push(i % 2 == 0);
        EXPECT_TRUE(q.size() == i + 1);
    }
    for (size_t i = 0; i < N; ++i) {
        EXPECT_TRUE(q.front() == (i % 2 == 0));
        q.pop();
    }
    EXPECT_TRUE(q.empty());
}

/// @brief Tests popping from an empty queue does not crash or change state.
/// @details Test Design: Robustness/Negative Testing.
TEST(FixQueueBool, PopEmpty)
{
    fix_queue_bool<2> q;
    q.pop(); // Should not crash
    EXPECT_TRUE(q.empty());
    q.push(true);
    q.pop();
    q.pop(); // Should not crash
    EXPECT_TRUE(q.empty());
}

/// @brief Tests fix_queue_bool with a container size of 400.
/// @details Test Design: Stress/Scalability. Fills the queue, checks FIFO order, and verifies wraparound and clear.
TEST(FixQueueBool, Size400)
{
    constexpr size_t N = 400;
    fix_queue_bool<N> q;

    // Initially empty
    EXPECT_TRUE(q.empty());
    EXPECT_TRUE(q.size() == 0);

    // Fill with alternating true/false
    for (size_t i = 0; i < N; ++i) {
        q.push(i % 2 == 0);
        EXPECT_TRUE(q.size() == i + 1);
        EXPECT_TRUE(q.back() == (i % 2 == 0));
    }
    EXPECT_TRUE(q.size() == N);
    EXPECT_FALSE(q.empty());

    // Check all values in FIFO order
    for (size_t i = 0; i < N; ++i) {
        EXPECT_TRUE(q.front() == (i % 2 == 0));
        q.pop();
        EXPECT_TRUE(q.size() == N - i - 1);
    }
    EXPECT_TRUE(q.empty());

    // Fill again and clear
    for (size_t i = 0; i < N; ++i) {
        q.push(true);
    }
    EXPECT_TRUE(q.size() == N);
    q.container.clear();
    EXPECT_TRUE(q.empty());
    EXPECT_TRUE(q.size() == 0);
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
  RUN_TEST(push_pop_1);
  RUN_TEST(push_pop_2);
  RUN_TEST(push_pop_3);
  RUN_TEST(BasicOperations);
  RUN_TEST(Capacity);
  RUN_TEST(Wraparound);
  RUN_TEST(LargeQueue);
  RUN_TEST(PopEmpty);
  RUN_TEST(Size400);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
