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
