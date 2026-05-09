/**
 * @file Ut_Signal/Test.cpp
 *
 * @brief Unit tests for class LedRouter (of project signal)
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
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
#include <Test/Logger.h>
#ifdef ARDUINO
#include <Arduino.h>
#endif

// ------------------------------------------------------------------------------------------------
/// Used types
// ------------------------------------------------------------------------------------------------
//using ramp_onboard_array_type = signal::LedRouter::ramp_onboard_array_type;
using ramp_base_type          = signal::LedRouter::ramp_base_type;
using target_type             = struct signal::target;
using intensity16_type        = signal::LedRouter::intensity16_type;
using speed16_ms_type         = signal::LedRouter::speed16_ms_type;
using intensity8_255          = util::intensity8_255;
using intensity8              = util::intensity8;

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
  target_type tgt(0);

  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    intensity16_type intensity;
    speed16_ms_type slope;
    intensity8_255 expectedPwm;
  } step_type;

  const util::array<step_type, 2> aSteps = 
  { {
      {   0,  intensity16_type::intensity_100(), util::kSpeed16Max, intensity8_255::intensity_100() }
    , {  10,  intensity16_type::intensity_0  (), util::kSpeed16Max, intensity8_255::intensity_0  () }
  } };

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.pin = 0; tgt.pin < cfg::kNrOnboardTargets; tgt.pin++)
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
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.pin, pwm);
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
  target_type tgt(0);

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
        {  0, convert_intensity_to_16( 0), util::kSpeed16Max, static_cast<intensity8_255>(0) }
      , { 10, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(2) }
      , { 20, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(2) }
      , { 30, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(4) }
      , { 40, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(6) }
      , { 50, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(9) }
      , { 60, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(13) }
      , { 70, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(16) }
      , { 80, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(16) }
      , { 90, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(16) }
      , {100, convert_intensity_to_16(50), 0x0100          , static_cast<intensity8_255>(16) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_0_50_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.pin = 0; tgt.pin < cfg::kNrOnboardTargets; tgt.pin++)
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
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.pin, pwm);
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
  target_type tgt(0);

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
        {  0, convert_intensity_to_16(50), util::kSpeed16Max, static_cast<intensity8_255>(16) }
      , { 10, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(10) }
      , { 20, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(7) }
      , { 30, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(4) }
      , { 40, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(3) }
      , { 50, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(2) }
      , { 60, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(1) }
      , { 70, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(0) }
      , { 80, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(0) }
      , { 90, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(0) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_50_0_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.pin = 0; tgt.pin < cfg::kNrOnboardTargets; tgt.pin++)
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
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.pin, pwm);
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
  target_type tgt(0);

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
        {  0, convert_intensity_to_16(100), util::kSpeed16Max, static_cast<intensity8_255>(255) }
      , { 10, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(165) }
      , { 20, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(107) }
      , { 30, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(69) }
      , { 40, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(45) }
      , { 50, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(29) }
      , { 60, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(19) }
      , { 70, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(12) }
      , { 80, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(8) }
      , { 90, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(5) }
      , {100, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(3) }
      , {110, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(2) }
      , {120, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(1) }
      , {130, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(0) }
      , {140, convert_intensity_to_16( 0), 0x0100          , static_cast<intensity8_255>(0) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_100_0_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.pin = 0; tgt.pin < cfg::kNrOnboardTargets; tgt.pin++)
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
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.pin, pwm);
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
  target_type tgt(0);

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
        {  0, convert_intensity_to_16(  0), util::kSpeed16Max, static_cast<intensity8_255>(0) }
      , { 10, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(2) }
      , { 20, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(2) }
      , { 30, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(4) }
      , { 40, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(6) }
      , { 50, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(9) }
      , { 60, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(13) }
      , { 70, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(21) }
      , { 80, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(32) }
      , { 90, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(49) }
      , {100, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(76) }
      , {110, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(117) }
      , {120, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(180) }
      , {130, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(255) }
      , {140, convert_intensity_to_16(100), 0x0100          , static_cast<intensity8_255>(255) }
  } };

  if (doLog)
  {
    log.start("setIntensityAndSpeed_0_100_0x0100.txt");
  }

  // For each onboard target
  tgt.type = target_type::kOnboard;
  for (tgt.pin = 0; tgt.pin < cfg::kNrOnboardTargets; tgt.pin++)
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
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.pin, pwm);
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
