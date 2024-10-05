/**
 * @file Ut_Signal/Test.cpp
 *
 * @brief Unit tests for project Signal
 *
 * @copyright Copyright 2022 - 2024 Ralf Sondershaus
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
#include <Test/Logger.h>
#include <Cal/CalM.h>
#include <Hal/Timer.h>
#include <Hal/Gpio.h>
#include <Rte/Rte.h>
#include <InputClassifier.h>
#include <Util/Array.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#define PRINT_RTE     0

#include "CalM_config.h"

typedef util::input_classifier<cfg::kNrClassifiers, cfg::kNrClassifierClasses> input_classifier_type;

// ---------------------------------------------------------------------------
/// Set elements of RTE arrays to 0
// ---------------------------------------------------------------------------
static void cleanRte()
{
  rte::classified_values_array classified_array;
  //rte::signal_intensity_array_type signal_intensities;
  rte::onboard_target_array onboard_targets;

  for (auto it = classified_array.begin(); it != classified_array.end(); it++)
  {
    *it = 0;
  }
  rte::ifc_classified_values::write(classified_array);

  for (auto it = onboard_targets.begin(); it != onboard_targets.end(); it++)
  {
    *it = 0;
  }
  rte::ifc_onboard_target_duty_cycles::write(onboard_targets);
}

// ---------------------------------------------------------------------------
/// Print elements of RTE to serial interface or stdout
// ---------------------------------------------------------------------------
static void printRte()
{
#if PRINT_RTE == 1
  rte::classified_values_array classified_array;
  rte::onboard_target_array onboard_targets;

  rte::ifc_classified_values::read(classified_array);
  rte::ifc_onboard_target_duty_cycles::read(onboard_targets);

#ifdef ARDUINO
  Serial.print("ifc_classified_values: ");
  for (auto it = classified_array.begin(); it != classified_array.end(); it++)
  {
    Serial.print(*it);
    Serial.print(" ");
  }
  Serial.println();

  Serial.println("ifc_onboard_target_duty_cycles: ");
  for (auto it = onboard_targets.begin(); it != onboard_targets.end(); it++)
  {
    Serial.print(*it);
    Serial.print(" ");
  }
  Serial.println();
#endif // ARDUINO
#endif // PRINT_RTE
}

// ------------------------------------------------------------------------------------------------
/// Test if aspects and dim ramps are ok for
/// - Commands from classifiers
/// - Start with default (init) aspect
/// - Switch to green
/// - Switch to red
// ------------------------------------------------------------------------------------------------
TEST(Ut_Signal, Default_Green_Red)
{
  typedef util::array<uint8, cfg::kNrSignalTargets> signal_target_array;
  using size_type = signal_target_array::size_type;
  using time_type = input_classifier_type::classifier_type::time_type;
  using cmd_type = rte::cmd_type;

  Logger log;

  typedef struct
  {
    time_type ms; // [ms] current time
    int nPin;      // input pin for AD value
    int nAdc;     // current AD value for pin
    cmd_type cmd; // expected value: command on RTE
    signal_target_array au8Curs; // expected value: target onboard duty cycles
  } step_type;

  const step_type aSteps[] =
  {
      {   0,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , {  10,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , {  20,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , {  30,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , {  40,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , {  50,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 16,  16,   0,   0,   0 } }
    , {  60,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 27,  27,   0,   0,   0 } }
    , {  70,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 48,  48,   0,   0,   0 } }
    , {  80,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 82,  82,   0,   0,   0 } }
    , {  90,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {145, 145,   0,   0,   0 } }
    , { 100,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {250, 250,   0,   0,   0 } }
    , { 510,  CLASSIFIER_PIN0, kGreenLo, rte::kInvalidCmd, {255, 255,   0,   0,   0 }}
    , { 560,  CLASSIFIER_PIN0, kGreenLo,                1, {145, 145,   0,   0,   0 }}
    , { 570,  CLASSIFIER_PIN0, kGreenLo,                1, { 84,  84,   0,   0,   0 }}
    , { 580,  CLASSIFIER_PIN0, kGreenLo,                1, { 48,  48,   0,   0,   0 }}
    , { 590,  CLASSIFIER_PIN0, kGreenLo,                1, { 28,  28,   0,   0,   0 }}
    , { 600,  CLASSIFIER_PIN0, kGreenLo,                1, { 16,  16,   0,   0,   0 }}
    , { 610,  CLASSIFIER_PIN0, kGreenLo,                1, {  9,   9,   0,   0,   0 }}
    , { 620,  CLASSIFIER_PIN0, kGreenLo,                1, {  5,   5,   0,   0,   0 }}
    , { 630,  CLASSIFIER_PIN0, kGreenLo,                1, {  3,   3,   0,   0,   0 }}
    , { 640,  CLASSIFIER_PIN0, kGreenLo,                1, {  2,   2,   0,   0,   0 }}
    , { 650,  CLASSIFIER_PIN0, kGreenLo,                1, {  0,   0,   0,   0,   0 }}
    , { 660,  CLASSIFIER_PIN0, kGreenLo,                1, {  0,   0,   2,   0,   0 }}
    , { 670,  CLASSIFIER_PIN0, kGreenLo,                1, {  0,   0,   3,   0,   0 }}
    , { 680,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   5,   0,   0 } }
    , { 690,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   9,   0,   0 } }
    , { 700,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  16,   0,   0 } }
    , { 710,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  27,   0,   0 } }
    , { 720,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  48,   0,   0 } }
    , { 730,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  82,   0,   0 } }
    , { 740,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 145,   0,   0 } }
    , { 750,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 250,   0,   0 } }
    , { 760,  CLASSIFIER_PIN0,   kRedLo, rte::kInvalidCmd, {  0,   0, 255,   0,   0 } }
    , { 810,  CLASSIFIER_PIN0,   kRedLo,                0, {  0,   0, 145,   0,   0 } }
    , { 820,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  84,   0,   0 } }
    , { 830,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  48,   0,   0 } }
    , { 840,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  28,   0,   0 } }
    , { 850,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  16,   0,   0 } }
    , { 860,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   9,   0,   0 } }
    , { 870,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   5,   0,   0 } }
    , { 880,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   3,   0,   0 } }
    , { 890,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   2,   0,   0 } }
    , { 900,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , { 910,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , { 920,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , { 930,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , { 940,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , { 950,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 16,  16,   0,   0,   0 } }
    , { 960,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 27,  27,   0,   0,   0 } }
    , { 970,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 48,  48,   0,   0,   0 } }
    , { 980,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 82,  82,   0,   0,   0 } }
    , { 990,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {145, 145,   0,   0,   0 } }
    , {1000,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {250, 250,   0,   0,   0 } }
  };

  input_classifier_type classifiers;
  cal::input_type in;
  size_t nStep;
  constexpr int kSignalId = 0;

  log.start("Default_Green_Red.txt");

  hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
  hal::stubs::millis = aSteps[0].ms;
  hal::stubs::micros = 1000U * hal::stubs::millis;
  rte::start();

  in.bits.type = cal::input_type::kClassified;
  in.bits.idx = 0;

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    hal::stubs::analogRead[aSteps[nStep].nPin] = aSteps[nStep].nAdc;
    hal::stubs::millis = aSteps[nStep].ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    rte::exec();
    printRte();
    rte::cmd_type cmd = rte::ifc_rte_get_cmd::call(in);
    log << std::setw(3) << (int)cmd << " ";
    EXPECT_EQ(cmd, aSteps[nStep].cmd);
    for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
    {
      rte::intensity8_255 pwm;
      rte::Ifc_OnboardTargetDutyCycles::size_type pos = static_cast<rte::Ifc_OnboardTargetDutyCycles::size_type>(rte::calib_mgr.signals[kSignalId].targets[i].idx);
      rte::ifc_onboard_target_duty_cycles::readElement(pos, pwm);
      log << std::setw(3) << (int)pwm << ", ";
      EXPECT_EQ((uint8)pwm, aSteps[nStep].au8Curs[i]);
    }
    log << std::endl;
  }

  log.stop();
}

void setUp(void)
{
  cleanRte();
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

  RUN_TEST(Default_Green_Red);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
