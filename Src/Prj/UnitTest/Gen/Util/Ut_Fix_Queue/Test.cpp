/**
  * @file Test.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief Google Test for Gen/Util/Fix_Queue.h
  *
  * @copyright Copyright 2023 Ralf Sondershaus
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

#include <Util/Fix_Queue.h>
#include <gtest/gtest.h>

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
