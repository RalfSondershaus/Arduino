/**
 * @file test.cpp
 *
 * @author Ralf Sondershaus
 *
 * Google Test for Gen/Util/bitset.h
 */

#include <Util/bitset.h>
#include <gtest/gtest.h>

TEST(Ut_Bitset, bitset_uint32_32_set)
{
  constexpr int nbits = 32;
  util::bitset<uint32, nbits> mybits(1U);

  EXPECT_EQ(mybits.test(0), true);
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), false);

  mybits.set(0, false);
  EXPECT_EQ(mybits.test(0), false);
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t bit = 0; bit < nbits; bit++)
  {
    mybits.set(bit, true);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), (i == bit) ? true : false);
      EXPECT_EQ(mybits[i], (i == bit) ? true : false);
    }
    EXPECT_EQ(mybits.any(), true);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), false);

    mybits.set(bit, false);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), false);
      EXPECT_EQ(mybits[i], false);
    }
    EXPECT_EQ(mybits.any(), false);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), true);
  }
}

TEST(Ut_Bitset, bitset_uint32_32_set_reset)
{
  constexpr int nbits = 32;
  util::bitset<uint32, nbits> mybits;

  (void)mybits.set();
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), true);
  EXPECT_EQ(mybits.none(), false);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), true);
    EXPECT_EQ(mybits[i], true);
  }

  (void)mybits.reset();
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), false);
    EXPECT_EQ(mybits[i], false);
  }
}

TEST(Ut_Bitset, bitset_uint16_16_set)
{
  constexpr int nbits = 16;
  util::bitset<uint16, nbits> mybits(1U);
  EXPECT_EQ(mybits.test(0), true);
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), false);

  mybits.set(0, false);
  EXPECT_EQ(mybits.test(0), false);
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t bit = 0; bit < nbits; bit++)
  {
    mybits.set(bit, true);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), (i == bit) ? true : false);
      EXPECT_EQ(mybits[i], (i == bit) ? true : false);
    }
    EXPECT_EQ(mybits.any(), true);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), false);

    mybits.set(bit, false);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), false);
      EXPECT_EQ(mybits[i], false);
    }
    EXPECT_EQ(mybits.any(), false);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), true);
  }
}

TEST(Ut_Bitset, bitset_uint16_16_set_reset)
{
  constexpr int nbits = 16;
  util::bitset<uint16, nbits> mybits;

  (void)mybits.set();
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), true);
  EXPECT_EQ(mybits.none(), false);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), true);
    EXPECT_EQ(mybits[i], true);
  }

  (void)mybits.reset();
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), false);
    EXPECT_EQ(mybits[i], false);
  }
}

TEST(Ut_Bitset, bitset_uint8_8_set)
{
  constexpr int nbits = 8;
  util::bitset<uint8, nbits> mybits(1U);

  EXPECT_EQ(mybits.test(0), true);
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), false);

  mybits.set(0, false);
  EXPECT_EQ(mybits.test(0), false);
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t bit = 0; bit < nbits; bit++)
  {
    mybits.set(bit, true);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), (i == bit) ? true : false);
      EXPECT_EQ(mybits[i], (i == bit) ? true : false);
    }
    EXPECT_EQ(mybits.any(), true);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), false);

    mybits.set(bit, false);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), false);
      EXPECT_EQ(mybits[i], false);
    }
    EXPECT_EQ(mybits.any(), false);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), true);
  }
}

TEST(Ut_Bitset, bitset_uint8_8_set_reset)
{
  constexpr int nbits = 8;
  util::bitset<uint8, nbits> mybits;

  (void)mybits.set();
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), true);
  EXPECT_EQ(mybits.none(), false);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), true);
    EXPECT_EQ(mybits[i], true);
  }

  (void)mybits.reset();
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), false);
    EXPECT_EQ(mybits[i], false);
  }
}

TEST(Ut_Bitset, bitset_uint32_64_set)
{
  constexpr int nbits = 64;
  util::bitset<uint32, nbits> mybits(1U);

  EXPECT_EQ(mybits.test(0), true);
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), false);

  mybits.set(0, false);
  EXPECT_EQ(mybits.test(0), false);
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t bit = 0; bit < nbits; bit++)
  {
    mybits.set(bit, true);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), (i == bit) ? true : false);
      EXPECT_EQ(mybits[i], (i == bit) ? true : false);
    }
    EXPECT_EQ(mybits.any(), true);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), false);

    mybits.set(bit, false);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), false);
      EXPECT_EQ(mybits[i], false);
    }
    EXPECT_EQ(mybits.any(), false);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), true);
  }
}

TEST(Ut_Bitset, bitset_uint32_64_set_reset)
{
  constexpr int nbits = 64;
  util::bitset<uint32, nbits> mybits;

  (void)mybits.set();
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), true);
  EXPECT_EQ(mybits.none(), false);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), true);
    EXPECT_EQ(mybits[i], true);
  }

  (void)mybits.reset();
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), false);
    EXPECT_EQ(mybits[i], false);
  }
}

TEST(Ut_Bitset, bitset_uint8_64_set)
{
  constexpr int nbits = 64;
  util::bitset<uint8, nbits> mybits(1U);

  EXPECT_EQ(mybits.test(0), true);
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), false);

  mybits.set(0, false);
  EXPECT_EQ(mybits.test(0), false);
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t bit = 0; bit < nbits; bit++)
  {
    mybits.set(bit, true);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), (i == bit) ? true : false);
      EXPECT_EQ(mybits[i], (i == bit) ? true : false);
    }
    EXPECT_EQ(mybits.any(), true);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), false);

    mybits.set(bit, false);
    for (size_t i = 0; i < nbits; i++)
    {
      EXPECT_EQ(mybits.test(i), false);
      EXPECT_EQ(mybits[i], false);
    }
    EXPECT_EQ(mybits.any(), false);
    EXPECT_EQ(mybits.all(), false);
    EXPECT_EQ(mybits.none(), true);
  }
}

TEST(Ut_Bitset, bitset_uint8_64_set_reset)
{
  constexpr int nbits = 64;
  util::bitset<uint8, nbits> mybits;

  (void)mybits.set();
  EXPECT_EQ(mybits.any(), true);
  EXPECT_EQ(mybits.all(), true);
  EXPECT_EQ(mybits.none(), false);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), true);
    EXPECT_EQ(mybits[i], true);
  }

  (void)mybits.reset();
  EXPECT_EQ(mybits.any(), false);
  EXPECT_EQ(mybits.all(), false);
  EXPECT_EQ(mybits.none(), true);

  for (size_t i = 0; i < nbits; i++)
  {
    EXPECT_EQ(mybits.test(i), false);
    EXPECT_EQ(mybits[i], false);
  }
}
