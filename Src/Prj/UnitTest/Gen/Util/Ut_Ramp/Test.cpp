/**
 * @file Ut_Ramp/Test.cpp
 *
 * @brief Unit tests for class Ramp of Gen/Util/Ramp.h
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
#include <Util/Ramp.h>
#include <Util/Math.h>

#include <ios> // for Logger on Windows
#include <fstream> // for Logger on Windows


// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
class Logger : public std::ofstream
{
public:
  void start(const std::string& filename)
  {
    open(filename, std::ios::out);
  }
  void stop()
  {
    close();
  }
};

template<class T>
static void test_ramp(const int cycleTime, const T startIntensity, const T endIntensity, const T slope, const bool doLog)
{
  using ramp_base_type = T;
  using ramp_type = typename util::ramp<ramp_base_type>;
  /// 16 bit, [./ms], 0 = zero speed, 65535 = 65535 / ms
  /// If used for intensity:
  /// - 0x8000 / ms is 100% / ms (fastest, see also kSpeed16Max)
  /// - 0x4000 / ms is  50% / ms
  /// - 0x0001 / ms is  0.000030517% / ms = 0.03% / s = 1.83% / min (slowest)
  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    ramp_base_type expectedCur;
  } step_type;

  Logger log;
  ramp_type myRamp;

  ramp_base_type curInc;
  if (util::math::mul_overflow<ramp_base_type>(slope, cycleTime, &curInc))
  {
    curInc = platform::numeric_limits<ramp_base_type>::max_();
  }
  const int nrSteps = (0x8000 / curInc) + 2;
  ramp_base_type expectedCur;
  int ms;

  if (doLog)
  {
    log.start("test_ramp.txt");
  }

  myRamp.init_from_slope(endIntensity, slope, cycleTime);

  EXPECT_EQ(myRamp.get_tgt(), endIntensity);
  EXPECT_EQ(myRamp.get_cur(), static_cast<ramp_base_type>(0));

  ms = 0;
  expectedCur = 0;
  // for each step
  for (int step = 0; step < nrSteps; step++)
  {
    ms += cycleTime;
    // expectedCur += curInc;
    if (util::math::add_overflow<ramp_base_type>(expectedCur, curInc, &expectedCur))
    {
      expectedCur = endIntensity;
    }
    expectedCur = util::min(endIntensity, expectedCur);
    const ramp_base_type cur = myRamp.step();
    if (doLog)
    {
      log << ms << " " << static_cast<int>(cur) << std::endl;
    }
    EXPECT_EQ(static_cast<int>(cur)             , static_cast<int>(expectedCur));
    EXPECT_EQ(static_cast<int>(myRamp.get_cur()), static_cast<int>(expectedCur));
  }

  if (doLog)
  {
    log.stop();
  }
}
 
// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     10
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x0001 (slowest).
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_10_0x0_0x8000_0x0001)
{
  test_ramp<uint16_t>(10, 0x0, 0x8000, 0x0001, false);
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     10
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x0010
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_10_0x0_0x8000_0x0010)
{
  test_ramp<uint16_t>(10, 0x0, 0x8000, 0x0010, false);
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     10
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x0100
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_10_0x0_0x8000_0x0100)
{
  test_ramp<uint16_t>(10, 0x0, 0x8000, 0x0100, false);
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     10
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x8000 (fastest).
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_10_0x0_0x8000_0x8000)
{
  using ramp_base_type = uint16_t;
  using ramp_type = typename util::ramp<ramp_base_type>;
  /// 16 bit, [./ms], 0 = zero speed, 65535 = 65535 / ms
  /// If used for intensity:
  /// - 0x8000 / ms is 100% / ms (fastest, see also kSpeed16Max)
  /// - 0x4000 / ms is  50% / ms
  /// - 0x0001 / ms is  0.000030517% / ms = 0.03% / s = 1.83% / min (slowest)
  /// - 0x0100 / ms = 0x0100 / 0x8000 % / ms = 0,0078125% / ms ~ 128 ms
  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    ramp_base_type expectedCur;
  } step_type;

  static constexpr int kCycleTime = 10;

  ramp_type myRamp;
  ramp_base_type intensity = 0x8000;
  ramp_base_type slope = 0x8000;

  const util::array<step_type, 2> aSteps = 
  { {
        { .ms = 10, .expectedCur = 32768 }
      , { .ms = 20, .expectedCur = 32768 }
  } };

  myRamp.init_from_slope(intensity, slope, kCycleTime);

  EXPECT_EQ(myRamp.get_tgt(), intensity);
  EXPECT_EQ(myRamp.get_cur(), static_cast<ramp_base_type>(0));

  // for each step
  for (auto step = aSteps.begin(); step != aSteps.end(); step++)
  {
    const ramp_base_type cur = myRamp.step();
    EXPECT_EQ(static_cast<int>(cur)             , static_cast<int>(step->expectedCur));
    EXPECT_EQ(static_cast<int>(myRamp.get_cur()), static_cast<int>(step->expectedCur));
  }
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     20
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x0001 (slowest).
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_20_0x0_0x8000_0x0001)
{
  test_ramp<uint16_t>(20, 0x0, 0x8000, 0x0001, false);
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     20
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x0010
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_20_0x0_0x8000_0x0010)
{
  test_ramp<uint16_t>(20, 0x0, 0x8000, 0x0010, false);
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     20
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x0100
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_20_0x0_0x8000_0x0100)
{
  test_ramp<uint16_t>(20, 0x0, 0x8000, 0x0100, false);
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     20
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x1000
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_20_0x0_0x8000_0x1000)
{
  test_ramp<uint16_t>(20, 0x0, 0x8000, 0x1000, true);
}

// ------------------------------------------------------------------------------------------------
/// Test with
/// - uint16
/// - cycle time     20
/// - Start          0% (0x0)
/// - End          100% (0x8000)
/// - Speed      0x8000
// ------------------------------------------------------------------------------------------------
TEST(Ut_Ramp, do_ramp_16bit_20_0x0_0x8000_0x8000)
{
  test_ramp<uint16_t>(20, 0x0, 0x8000, 0x8000, false);
}

void setUp(void)
{
}

void tearDown(void)
{
}

int main(void)
{
  UNITY_BEGIN();

  RUN_TEST(do_ramp_16bit_10_0x0_0x8000_0x0001);
  RUN_TEST(do_ramp_16bit_10_0x0_0x8000_0x0010);
  RUN_TEST(do_ramp_16bit_10_0x0_0x8000_0x0100);
  RUN_TEST(do_ramp_16bit_10_0x0_0x8000_0x8000);
  RUN_TEST(do_ramp_16bit_20_0x0_0x8000_0x0001);
  RUN_TEST(do_ramp_16bit_20_0x0_0x8000_0x0010);
  RUN_TEST(do_ramp_16bit_20_0x0_0x8000_0x0100);
  RUN_TEST(do_ramp_16bit_20_0x0_0x8000_0x1000);
  RUN_TEST(do_ramp_16bit_20_0x0_0x8000_0x8000);

  return UNITY_END();
}
