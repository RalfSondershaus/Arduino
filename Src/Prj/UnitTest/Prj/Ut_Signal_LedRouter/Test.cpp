/**
 * @file Ut_Signal/Test.cpp
 *
 * @brief Unit tests for class LedRouter (of project signal)
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

// Avoid to use standard C++ libraries because the code shall run on Arduinos
// with avr gcc but avr gcc doesn't support many standard C++ libraries.

// To make Unity colorize the output:
// - #define UNITY_OUTPUT_COLOR (in unity.h)
// - Modify UnityStrFail etc in unity.c
//   \033[       ESC sequence for Control Sequence Introducer (CSI)
//   \033[nm     CSI for SGR (Select Graphic Rendition)
//   n = 30-37   Set foreground color (e.g. \033[32m for green)
//   n = 40-47   Set background color (e.g. \033[41m for red)

#include <unity_adapt.h>
#include <Rte/Rte.h>
#include <LedRouter.h>
#include <Util/Array.h>
#include <Hal/Timer.h>
#ifdef WIN32
#include <ios> // for Logger on Windows
#include <fstream> // for Logger on Windows
#endif
#ifdef ARDUINO
#include <Arduino.h>
#endif

// ------------------------------------------------------------------------------------------------
/// Used types
// ------------------------------------------------------------------------------------------------
//using ramp_onboard_array_type = signal::LedRouter::ramp_onboard_array_type;
using ramp_base_type          = signal::LedRouter::ramp_base_type;
using target_type             = signal::LedRouter::target_type;
using intensity16_type        = signal::LedRouter::intensity16_type;
using speed16_ms_type         = signal::LedRouter::speed16_ms_type;
using intensity8_255          = rte::intensity8_255;
using intensity8              = rte::intensity8;

// ------------------------------------------------------------------------------------------------
/// Helper function: convert an intensity 0% ... 100% to a 16 bit intensity with 0x8000 = 100%.
///
/// r / 0x8000 = intensity / 100
/// r = intensity * 0x8000 / 100
// ------------------------------------------------------------------------------------------------
constexpr signal::LedRouter::intensity16_type convert_intensity_to_16(int intensity) 
{ 
  //                       int         uint16
  // type conversion:      int         int
  return intensity16_type((static_cast<uint32_t>(intensity) * intensity16_type::intensity_100()) / 100);
}

#ifdef WIN32
// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
class Logger : public std::ofstream
{
public:
  void start(const std::string& filename)
  {
    open(filename);
  }
  void stop()
  {
    close();
  }
};
#else
// ------------------------------------------------------------------------------------------------
/// This dummy logger for Arduino is doing nothing.
// ------------------------------------------------------------------------------------------------
class Logger
{
public:
  void start(const char* filename)
  {
  }
  void stop()
  {
  }
  Logger& operator<<(uint16) { return *this; }
  Logger& operator<<(const char *) { return *this; }
};

namespace std
{
  // simple fix for std::endl on Arduino
  constexpr char endl = '\n';
}
#endif

// ------------------------------------------------------------------------------------------------
/// Test if dim ramps and gamma correction are ok for
/// - setIntensityAndSpeed
///
/// Check for every onboard target:
/// - 100% with max speed (slope); overflow protection in ramps is tested implicitly here
/// - 0% with max speed
// ------------------------------------------------------------------------------------------------
TEST(Ut_LedRouter, setIntensityAndSpeed_100_0x8000)
{
  signal::LedRouter router;
  target_type tgt;

  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    intensity16_type intensity;
    speed16_ms_type slope;
    intensity8_255 expectedPwm;
  } step_type;

  const util::array<step_type, 2> aSteps = 
  { {
      {   0,  intensity16_type::intensity_100(), rte::kSpeed16Max, intensity8_255::intensity_100() }
    , {  10,  intensity16_type::intensity_0  (), rte::kSpeed16Max, intensity8_255::intensity_0  () }
  } };

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.idx = 0; tgt.idx < cfg::kNrOnboardTargets; tgt.idx++)
  {
    router.init();
    // ... and for each step
    for (auto step = aSteps.begin(); step != aSteps.end(); step++)
    {
      // ... set target intensity and check output target intensity
      intensity8_255 pwm;
      hal::stubs::millis = step->ms;
      hal::stubs::micros = 1000U * hal::stubs::millis;
      router.setIntensityAndSpeed(tgt, step->intensity, step->slope);
      router.cycle();
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.idx, pwm);
      EXPECT_EQ(static_cast<int>(pwm), static_cast<int>(step->expectedPwm));
    }
  }
}

// ------------------------------------------------------------------------------------------------
/// Test if dim ramps and gamma correction are ok for
/// - setIntensityAndSpeed
///
/// Check for every onboard target:
/// - Start with 0%
/// - 50% with slope 0x0100 (128 ms from 0% to 100%, so ca. 74 ms from 0% to 50%)
// ------------------------------------------------------------------------------------------------
TEST(Ut_LedRouter, setIntensityAndSpeed_0_50_0x0100)
{
  Logger log;
  signal::LedRouter router;
  target_type tgt;

  const bool doLog = false;

  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    intensity16_type intensity;
    speed16_ms_type slope;
    intensity8_255 expectedPwm;
  } step_type;

  const util::array<step_type, 11> aSteps = 
  { {
        { .ms =  0, .intensity = convert_intensity_to_16( 0), .slope = rte::kSpeed16Max, .expectedPwm = static_cast<intensity8_255>(0) }
      , { .ms = 10, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 20, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 30, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(4) }
      , { .ms = 40, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(6) }
      , { .ms = 50, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(9) }
      , { .ms = 60, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(13) }
      , { .ms = 70, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(16) }
      , { .ms = 80, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(16) }
      , { .ms = 90, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(16) }
      , { .ms = 100, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(16) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_0_50_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.idx = 0; tgt.idx < cfg::kNrOnboardTargets; tgt.idx++)
  {
    router.init();
    // ... and for each step
    for (auto step = aSteps.begin(); step != aSteps.end(); step++)
    {
      // ... set target intensity and check output target intensity
      intensity8_255 pwm;
      hal::stubs::millis = step->ms;
      hal::stubs::micros = 1000U * hal::stubs::millis;
      router.setIntensityAndSpeed(tgt, step->intensity, step->slope);
      router.cycle();
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.idx, pwm);
      if (doLog)
      {
        log << step->ms << " " << static_cast<int>(pwm) << std::endl;
      }
      EXPECT_EQ(static_cast<int>(pwm), static_cast<int>(step->expectedPwm));
    }
  }
  if (doLog)
  {
    log.stop();
  }
}

// ------------------------------------------------------------------------------------------------
/// Test if dim ramps and gamma correction are ok for
/// - setIntensityAndSpeed
///
/// Check for every onboard target:
/// - Start with 50%
/// - 0% with slope 0x0100 (128 ms from 0% to 100%, so ca. 74 ms from 0% to 50%)
// ------------------------------------------------------------------------------------------------
TEST(Ut_LedRouter, setIntensityAndSpeed_50_0_0x0100)
{
  Logger log;
  signal::LedRouter router;
  target_type tgt;

  const bool doLog = false;

  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    intensity16_type intensity;
    speed16_ms_type slope;
    intensity8_255 expectedPwm;
  } step_type;

  const util::array<step_type, 10> aSteps = 
  { {
        { .ms =  0, .intensity = convert_intensity_to_16(50), .slope = rte::kSpeed16Max, .expectedPwm = static_cast<intensity8_255>(16) }
      , { .ms = 10, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(10) }
      , { .ms = 20, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(7) }
      , { .ms = 30, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(4) }
      , { .ms = 40, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 50, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 60, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(1) }
      , { .ms = 70, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(0) }
      , { .ms = 80, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(0) }
      , { .ms = 90, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(0) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_50_0_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.idx = 0; tgt.idx < cfg::kNrOnboardTargets; tgt.idx++)
  {
    router.init();
    // ... and for each step
    for (auto step = aSteps.begin(); step != aSteps.end(); step++)
    {
      // ... set target intensity and check output target intensity
      intensity8_255 pwm;
      hal::stubs::millis = step->ms;
      hal::stubs::micros = 1000U * hal::stubs::millis;
      router.setIntensityAndSpeed(tgt, step->intensity, step->slope);
      router.cycle();
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.idx, pwm);
      if (doLog)
      {
        log << step->ms << " " << static_cast<int>(pwm) << std::endl;
      }
      EXPECT_EQ(static_cast<int>(pwm), static_cast<int>(step->expectedPwm));
    }
  }
  if (doLog)
  {
    log.stop();
  }
}

// ------------------------------------------------------------------------------------------------
/// Test if dim ramps and gamma correction are ok for
/// - setIntensityAndSpeed
///
/// Check for every onboard target:
/// - Start with 100%
/// - 0% with slope 0x0100 (128 ms from 0% to 100%, so ca. 74 ms from 0% to 50%)
// ------------------------------------------------------------------------------------------------
TEST(Ut_LedRouter, setIntensityAndSpeed_100_0_0x0100)
{
  Logger log;
  signal::LedRouter router;
  target_type tgt;

  const bool doLog = false;

  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    intensity16_type intensity;
    speed16_ms_type slope;
    intensity8_255 expectedPwm;
  } step_type;

  const util::array<step_type, 15> aSteps = 
  { {
        { .ms =  0, .intensity = convert_intensity_to_16(100), .slope = rte::kSpeed16Max, .expectedPwm = static_cast<intensity8_255>(255) }
      , { .ms = 10, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(165) }
      , { .ms = 20, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(107) }
      , { .ms = 30, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(69) }
      , { .ms = 40, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(45) }
      , { .ms = 50, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(29) }
      , { .ms = 60, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(19) }
      , { .ms = 70, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(12) }
      , { .ms = 80, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(8) }
      , { .ms = 90, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(5) }
      , { .ms = 100, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 110, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 120, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(1) }
      , { .ms = 130, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(0) }
      , { .ms = 140, .intensity = convert_intensity_to_16( 0), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(0) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_100_0_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.idx = 0; tgt.idx < cfg::kNrOnboardTargets; tgt.idx++)
  {
    router.init();
    // ... and for each step
    for (auto step = aSteps.begin(); step != aSteps.end(); step++)
    {
      // ... set target intensity and check output target intensity
      intensity8_255 pwm;
      hal::stubs::millis = step->ms;
      hal::stubs::micros = 1000U * hal::stubs::millis;
      router.setIntensityAndSpeed(tgt, step->intensity, step->slope);
      router.cycle();
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.idx, pwm);
      if (doLog)
      {
        log << step->ms << " " << static_cast<int>(pwm) << std::endl;
      }
      EXPECT_EQ(static_cast<int>(pwm), static_cast<int>(step->expectedPwm));
    }
  }
  if (doLog)
  {
    log.stop();
  }
}

// ------------------------------------------------------------------------------------------------
/// Test if dim ramps and gamma correction are ok for
/// - setIntensityAndSpeed
///
/// Check for every onboard target:
/// - Start with 100%
/// - 0% with slope 0x0100 (128 ms from 0% to 100%, so ca. 74 ms from 0% to 50%)
// ------------------------------------------------------------------------------------------------
TEST(Ut_LedRouter, setIntensityAndSpeed_0_100_0x0100)
{
  Logger log;
  signal::LedRouter router;
  target_type tgt;

  const bool doLog = false;

  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    intensity16_type intensity;
    speed16_ms_type slope;
    intensity8_255 expectedPwm;
  } step_type;

  const util::array<step_type, 15> aSteps = 
  { {
        { .ms =  0, .intensity = convert_intensity_to_16(0), .slope = rte::kSpeed16Max, .expectedPwm = static_cast<intensity8_255>(0) }
      , { .ms = 10, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 20, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 30, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(4) }
      , { .ms = 40, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(6) }
      , { .ms = 50, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(9) }
      , { .ms = 60, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(13) }
      , { .ms = 70, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(21) }
      , { .ms = 80, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(32) }
      , { .ms = 90, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(49) }
      , { .ms = 100, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(76) }
      , { .ms = 110, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(117) }
      , { .ms = 120, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(180) }
      , { .ms = 130, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(255) }
      , { .ms = 140, .intensity = convert_intensity_to_16(100), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(255) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_0_100_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.idx = 0; tgt.idx < cfg::kNrOnboardTargets; tgt.idx++)
  {
    router.init();
    // ... and for each step
    for (auto step = aSteps.begin(); step != aSteps.end(); step++)
    {
      // ... set target intensity and check output target intensity
      intensity8_255 pwm;
      hal::stubs::millis = step->ms;
      hal::stubs::micros = 1000U * hal::stubs::millis;
      router.setIntensityAndSpeed(tgt, step->intensity, step->slope);
      router.cycle();
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.idx, pwm);
      if (doLog)
      {
        log << step->ms << " " << static_cast<int>(pwm) << std::endl;
      }
      EXPECT_EQ(static_cast<int>(pwm), static_cast<int>(step->expectedPwm));
    }
  }
  if (doLog)
  {
    log.stop();
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

  RUN_TEST(setIntensityAndSpeed_100_0x8000);
  RUN_TEST(setIntensityAndSpeed_0_50_0x0100);
  RUN_TEST(setIntensityAndSpeed_50_0_0x0100);
  RUN_TEST(setIntensityAndSpeed_100_0_0x0100);
  RUN_TEST(setIntensityAndSpeed_0_100_0x0100);

  UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
