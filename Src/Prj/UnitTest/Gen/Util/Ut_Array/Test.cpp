/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/Array.h
 */

#include <Util/Array.h>
#include <unity_adapt.h>

typedef typename util::array<int, 5>::size_type size_type;

TEST(Ut_Array, array_uninit_begin_end)
{
  util::array<int, 5> arr;

  auto it = arr.begin();
  int n;

  EXPECT_EQ(arr.size(), size_type{ 5 });
  EXPECT_EQ(arr.max_size(), size_type{ 5 });

  n = 0;
  for (it = arr.begin(); it != arr.end(); it++)
  {
    n++;
  }
  EXPECT_EQ(n, 5);
}

TEST(Ut_Array, array_operator)
{
  util::array<int, 5> arr;
  int i;
  auto it = arr.begin();

  for (i = 0; i < static_cast<int>(arr.size()); i++)
  {
    arr[i] = i;
  }

  i = 0;
  for (it = arr.begin(); it != arr.end(); it++)
  {
    EXPECT_EQ(*it, i);
    EXPECT_EQ(arr[i], i);
    i++;
  }

}

TEST(Ut_Array, array_at)
{
  util::array<int, 5> arr;
  int i;
  auto it = arr.begin();

  for (i = 0; i < static_cast<int>(arr.size()); i++)
  {
    arr.at(i) = i;
  }

  i = 0;
  for (it = arr.begin(); it != arr.end(); it++)
  {
    EXPECT_EQ(*it, i);
    EXPECT_EQ(arr.at(i), i);
    i++;
  }
}

TEST(Ut_Array, array_front)
{
  util::array<int, 5> arr;

  arr.front() = 5;

  EXPECT_EQ(arr.front(), 5);
}

TEST(Ut_Array, array_back)
{
  util::array<int, 5> arr;

  arr.back() = 5;

  EXPECT_EQ(arr.back(), 5);
}

TEST(Ut_Array, array_fill)
{
  util::array<int, 5> arr;

  arr.fill(10);

  for (auto it = arr.begin(); it != arr.end(); it++)
  {
    EXPECT_EQ(*it, 10);
  }
}

TEST(Ut_Array, array_copy_constructor)
{
  util::array<int, 5> arr;

  arr.fill(10);

  for (auto it = arr.begin(); it != arr.end(); it++)
  {
    EXPECT_EQ(*it, 10);
  }

  auto arr_copy = arr; // copy initialization

  for (auto it = arr_copy.begin(); it != arr_copy.end(); it++)
  {
    EXPECT_EQ(*it, 10);
  }

}

TEST(Ut_Array, array_copy_assignment)
{
  util::array<int, 5> arr;
  util::array<int, 5> arr_copy;

  arr.fill(10);
  arr_copy.fill(20);

  for (auto it = arr.begin(); it != arr.end(); it++)
  {
    EXPECT_EQ(*it, 10);
  }

  arr_copy = arr; // copy assignment

  for (auto it = arr_copy.begin(); it != arr_copy.end(); it++)
  {
    EXPECT_EQ(*it, 10);
  }

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

  RUN_TEST(array_uninit_begin_end);
  RUN_TEST(array_at);
  RUN_TEST(array_front);
  RUN_TEST(array_back);
  RUN_TEST(array_fill);
  RUN_TEST(array_copy_constructor);
  RUN_TEST(array_copy_assignment);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
