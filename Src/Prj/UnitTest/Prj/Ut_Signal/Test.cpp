/**
 * @file Ut_Signal/Test.cpp
 *
 * @brief Unit tests for project Signal
 *
 * @copyright Copyright 2022 Ralf Sondershaus
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

#include <gtest/gtest.h>
#include <Cal/CalM.h>
#include <Rte/Rte.h>
#include <InputClassifier.h>
#include <array>
#include <algorithm>
#include <ios> // for Logger
#include <fstream> // for Logger

#include "CalM_config.h"

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

/// Stub variables
static unsigned long Arduino_Stub_MicrosReturnValue;
static unsigned long Arduino_Stub_MillisReturnValue;
static std::array<int, 255> Arduino_Stub_analogRead_ReturnValues;

/// Return current time [us]
unsigned long micros() { return Arduino_Stub_MicrosReturnValue; }
/// Return current time [ms]
unsigned long millis() { return Arduino_Stub_MillisReturnValue; }
/// Return analogue value
int analogRead(uint8 ucPin) { return Arduino_Stub_analogRead_ReturnValues[ucPin]; }

typedef util::input_classifier<cfg::kNrClassifiers, cfg::kNrClassifierClasses> input_classifier_type;

static void PreTest()
{
  std::fill(Arduino_Stub_analogRead_ReturnValues.begin(), Arduino_Stub_analogRead_ReturnValues.end(), 0);
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
  typedef std::array<uint8, cfg::kNrSignalTargets> signal_target_array;
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
    signal_target_array au8Curs; // expected value: target onboard intensities
  } step_type;

  const step_type aSteps[] =
  {
      {   0,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , {  10,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 25,  25,   0,   0,   0 } }
    , {  20,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 51,  51,   0,   0,   0 } }
    , {  30,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 76,  76,   0,   0,   0 } }
    , {  40,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {102, 102,   0,   0,   0 } }
    , {  50,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {127, 127,   0,   0,   0 } }
    , {  60,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {153, 153,   0,   0,   0 } }
    , {  70,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {178, 178,   0,   0,   0 } }
    , {  80,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {204, 204,   0,   0,   0 } }
    , {  90,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {229, 229,   0,   0,   0 } }
    , { 100,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {255, 255,   0,   0,   0 } }
    , { 510,  CLASSIFIER_PIN0, kGreenLo, rte::kInvalidCmd, {255, 255,   0,   0,   0 }}
    , { 560,  CLASSIFIER_PIN0, kGreenLo,                1, {229, 229,   0,   0,   0 }}
    , { 570,  CLASSIFIER_PIN0, kGreenLo,                1, {204, 204,   0,   0,   0 }}
    , { 580,  CLASSIFIER_PIN0, kGreenLo,                1, {178, 178,   0,   0,   0 }}
    , { 590,  CLASSIFIER_PIN0, kGreenLo,                1, {153, 153,   0,   0,   0 }}
    , { 600,  CLASSIFIER_PIN0, kGreenLo,                1, {127, 127,   0,   0,   0 }}
    , { 610,  CLASSIFIER_PIN0, kGreenLo,                1, {102, 102,   0,   0,   0 }}
    , { 620,  CLASSIFIER_PIN0, kGreenLo,                1, { 76,  76,   0,   0,   0 }}
    , { 630,  CLASSIFIER_PIN0, kGreenLo,                1, { 51,  51,   0,   0,   0 }}
    , { 640,  CLASSIFIER_PIN0, kGreenLo,                1, { 25,  25,   0,   0,   0 }}
    , { 650,  CLASSIFIER_PIN0, kGreenLo,                1, {  0,   0,   0,   0,   0 }}
    , { 660,  CLASSIFIER_PIN0, kGreenLo,                1, {  0,   0,  25,   0,   0 }}
    , { 670,  CLASSIFIER_PIN0, kGreenLo,                1, {  0,   0,  51,   0,   0 }}
    , { 680,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  76,   0,   0 } }
    , { 690,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 102,   0,   0 } }
    , { 700,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 127,   0,   0 } }
    , { 710,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 153,   0,   0 } }
    , { 720,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 178,   0,   0 } }
    , { 730,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 204,   0,   0 } }
    , { 740,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 229,   0,   0 } }
    , { 750,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 255,   0,   0 } }
    , { 760,  CLASSIFIER_PIN0,   kRedLo, rte::kInvalidCmd, {  0,   0, 255,   0,   0 } }
    , { 810,  CLASSIFIER_PIN0,   kRedLo,                0, {  0,   0, 229,   0,   0 } }
    , { 820,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 204,   0,   0 } }
    , { 830,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 178,   0,   0 } }
    , { 840,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 153,   0,   0 } }
    , { 850,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 127,   0,   0 } }
    , { 860,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0, 102,   0,   0 } }
    , { 870,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  76,   0,   0 } }
    , { 880,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  51,   0,   0 } }
    , { 890,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,  25,   0,   0 } }
    , { 900,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , { 910,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 25,  25,   0,   0,   0 } }
    , { 920,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 51,  51,   0,   0,   0 } }
    , { 930,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, { 76,  76,   0,   0,   0 } }
    , { 940,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {102, 102,   0,   0,   0 } }
    , { 950,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {127, 127,   0,   0,   0 } }
    , { 960,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {153, 153,   0,   0,   0 } }
    , { 970,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {178, 178,   0,   0,   0 } }
    , { 980,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {204, 204,   0,   0,   0 } }
    , { 990,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {229, 229,   0,   0,   0 } }
    , {1000,  CLASSIFIER_PIN0,        0, rte::kInvalidCmd, {255, 255,   0,   0,   0 } }
  };

  input_classifier_type classifiers;
  cal::input_type in;
  int nStep;
  constexpr int kSignalId = 0;

  log.start("InputClassifier_test_1.txt");

  PreTest();

  rte::start();

  in.type = cal::input_type::kClassified;
  in.idx = 0;

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    Arduino_Stub_analogRead_ReturnValues[aSteps[nStep].nPin] = aSteps[nStep].nAdc;
    Arduino_Stub_MillisReturnValue = aSteps[nStep].ms;
    Arduino_Stub_MicrosReturnValue = 1000U * Arduino_Stub_MillisReturnValue;
    rte::exec();
    rte::cmd_type cmd = rte::ifc_rte_get_cmd::call(in);
    log << std::setw(3) << (int)cmd << " ";
    EXPECT_EQ(cmd, aSteps[nStep].cmd);
    for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
    {
      rte::intensity8_t u8Intensity;
      rte::Ifc_OnboardTargetIntensities::size_type pos = static_cast<rte::Ifc_OnboardTargetIntensities::size_type>(rte::calib_mgr.signals[kSignalId].targets[i].idx);
      EXPECT_EQ(rte::ifc_onboard_target_intensities::readElement(pos, u8Intensity), rte::ret_type::OK);
      log << std::setw(3) << (int)u8Intensity << ", ";
      EXPECT_EQ(u8Intensity, aSteps[nStep].au8Curs[i]);
    }
    log << std::endl;
  }

  log.stop();
}
