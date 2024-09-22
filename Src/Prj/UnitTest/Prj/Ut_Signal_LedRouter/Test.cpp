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
#include <ios> // for Logger on Windows
#include <fstream> // for Logger on Windows


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
  return intensity16_type((intensity * intensity16_type::intensity_100()) / 100);
}

// ------------------------------------------------------------------------------------------------
// ------------------------------------------------------------------------------------------------
class Logger : public std::ofstream
{
public:
  void start(const std::string& filename)
  {
    open(filename, std::ios::trunc);
  }
  void stop()
  {
    close();
  }
};

// ------------------------------------------------------------------------------------------------
/// Test if aspects and dim ramps are ok for
/// - setIntensityAndSpeed
///
/// Check for every onboard target:
/// - 100% with max speed (slope); overflow protection in ramps is tested implicitly here
/// - 0% with max speed
// ------------------------------------------------------------------------------------------------
TEST(Ut_LedRouter, setIntensityAndSpeed_1)
{
  Logger log;
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

  //init();

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
      stubs::millis = step->ms;
      stubs::micros = 1000U * stubs::millis;
      router.setIntensityAndSpeed(tgt, step->intensity, step->slope);
      router.cycle();
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.idx, pwm);
      EXPECT_EQ(static_cast<int>(pwm), static_cast<int>(step->expectedPwm));
    }
  }
}

// ------------------------------------------------------------------------------------------------
/// Test if aspects and dim ramps are ok for
/// - setIntensityAndSpeed
///
/// Check for every onboard target:
/// - 100% with max speed (slope); overflow protection in ramps is tested implicitly here
/// - 0% with max speed
// ------------------------------------------------------------------------------------------------
TEST(Ut_LedRouter, setIntensityAndSpeed_2)
{
  Logger log;
  signal::LedRouter router;
  target_type tgt;

  /// 16 bit, [./ms], 0 = zero speed, 65535 = 65535 / ms
  /// If used for intensity:
  /// - 0x8000 / ms is 100% / ms (fastest, see also kSpeed16Max)
  /// - 0x4000 / ms is  50% / ms
  /// - 0x0001 / ms is  0.000030517% / ms = 0.03% / s = 1.83% / min (slowest)
  /// - 0x0100 / ms = 0x0100 / 0x8000 % / ms = 0,0078125% / ms ~ 128 ms
  typedef struct
  {
    ramp_base_type ms; // [ms] current time
    intensity16_type intensity;
    speed16_ms_type slope;
    intensity8_255 expectedPwm;
  } step_type;

  const util::array<step_type, 27> aSteps = 
  { {
        { .ms =  0, .intensity = convert_intensity_to_16( 0), .slope = rte::kSpeed16Max, .expectedPwm = static_cast<intensity8_255>(0) }
      , { .ms = 10, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 20, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(2) }
      , { .ms = 30, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 40, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 50, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 60, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 70, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 80, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 90, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 100, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 110, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 120, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 130, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 140, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 150, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 160, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 170, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 180, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 190, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 200, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 210, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 220, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 230, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 240, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 250, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
      , { .ms = 260, .intensity = convert_intensity_to_16(50), .slope = 0x0100          , .expectedPwm = static_cast<intensity8_255>(3) }
  } };

  log.start("setIntensityAndSpeed_2.txt");

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
      stubs::millis = step->ms;
      stubs::micros = 1000U * stubs::millis;
      router.setIntensityAndSpeed(tgt, step->intensity, step->slope);
      router.cycle();
      rte::ifc_onboard_target_duty_cycles::readElement(tgt.idx, pwm);
      log << step->ms << " " << static_cast<int>(pwm) << std::endl;
      //EXPECT_EQ(static_cast<int>(pwm), static_cast<int>(step->expectedPwm));
    }
  }
  log.stop();
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

  RUN_TEST(setIntensityAndSpeed_1);
  RUN_TEST(setIntensityAndSpeed_2);

  return UNITY_END();
}
