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
#include <Hal/EEPROM.h>
#include <Hal/Gpio.h>
#include <Hal/Timer.h>
#include <Rte/Rte.h>
#include <InputClassifier.h>
#include <Util/Array.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#define PRINT_RTE     0

#include <Cal/CalM_config.h>

typedef util::classifier_array<cfg::kNrClassifiers, cfg::kNrClassifierClasses> classifier_array_type;

static constexpr uint8 kBuiltInSignalIDAusfahrsignal = 1;

// ---------------------------------------------------------------------------
/// Set elements of RTE arrays to 0
// ---------------------------------------------------------------------------
static void cleanRte()
{
  rte::classified_values_array classified_array;
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
  using time_type = classifier_array_type::classifier_type::time_type;
  using cmd_type = rte::cmd_type;

  Logger log;
  constexpr int kSignalId = 0; // Need to configure signal 0 as Ausfahrsignal
  constexpr uint8 kFirstOutputPin = 13;
  constexpr uint8 kInputPin0 = 54;
  constexpr uint8 kClassifierType = 0;
  uint8 tmp;

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
      {   0,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , {  10,  kInputPin0,             0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , {  20,  kInputPin0,             0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , {  30,  kInputPin0,             0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , {  40,  kInputPin0,             0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , {  50,  kInputPin0,             0, rte::kInvalidCmd, { 16,  16,   0,   0,   0 } }
    , {  60,  kInputPin0,             0, rte::kInvalidCmd, { 27,  27,   0,   0,   0 } }
    , {  70,  kInputPin0,             0, rte::kInvalidCmd, { 48,  48,   0,   0,   0 } }
    , {  80,  kInputPin0,             0, rte::kInvalidCmd, { 82,  82,   0,   0,   0 } }
    , {  90,  kInputPin0,             0, rte::kInvalidCmd, {145, 145,   0,   0,   0 } }
    , { 100,  kInputPin0,             0, rte::kInvalidCmd, {250, 250,   0,   0,   0 } }
    , { 510,  kInputPin0, cal::kGreenLo, rte::kInvalidCmd, {255, 255,   0,   0,   0 }}
    , { 560,  kInputPin0, cal::kGreenLo,                1, {145, 145,   0,   0,   0 }}
    , { 570,  kInputPin0, cal::kGreenLo,                1, { 84,  84,   0,   0,   0 }}
    , { 580,  kInputPin0, cal::kGreenLo,                1, { 48,  48,   0,   0,   0 }}
    , { 590,  kInputPin0, cal::kGreenLo,                1, { 28,  28,   0,   0,   0 }}
    , { 600,  kInputPin0, cal::kGreenLo,                1, { 16,  16,   0,   0,   0 }}
    , { 610,  kInputPin0, cal::kGreenLo,                1, {  9,   9,   0,   0,   0 }}
    , { 620,  kInputPin0, cal::kGreenLo,                1, {  5,   5,   0,   0,   0 }}
    , { 630,  kInputPin0, cal::kGreenLo,                1, {  3,   3,   0,   0,   0 }}
    , { 640,  kInputPin0, cal::kGreenLo,                1, {  2,   2,   0,   0,   0 }}
    , { 650,  kInputPin0, cal::kGreenLo,                1, {  0,   0,   0,   0,   0 }}
    , { 660,  kInputPin0, cal::kGreenLo,                1, {  0,   0,   2,   0,   0 }}
    , { 670,  kInputPin0, cal::kGreenLo,                1, {  0,   0,   3,   0,   0 }}
    , { 680,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   5,   0,   0 } }
    , { 690,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   9,   0,   0 } }
    , { 700,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  16,   0,   0 } }
    , { 710,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  27,   0,   0 } }
    , { 720,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  48,   0,   0 } }
    , { 730,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  82,   0,   0 } }
    , { 740,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0, 145,   0,   0 } }
    , { 750,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0, 250,   0,   0 } }
    , { 760,  kInputPin0,   cal::kRedLo, rte::kInvalidCmd, {  0,   0, 255,   0,   0 } }
    , { 810,  kInputPin0,   cal::kRedLo,                0, {  0,   0, 145,   0,   0 } }
    , { 820,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  84,   0,   0 } }
    , { 830,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  48,   0,   0 } }
    , { 840,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  28,   0,   0 } }
    , { 850,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,  16,   0,   0 } }
    , { 860,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   9,   0,   0 } }
    , { 870,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   5,   0,   0 } }
    , { 880,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   3,   0,   0 } }
    , { 890,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   2,   0,   0 } }
    , { 900,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , { 910,  kInputPin0,             0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , { 920,  kInputPin0,             0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , { 930,  kInputPin0,             0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , { 940,  kInputPin0,             0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , { 950,  kInputPin0,             0, rte::kInvalidCmd, { 16,  16,   0,   0,   0 } }
    , { 960,  kInputPin0,             0, rte::kInvalidCmd, { 27,  27,   0,   0,   0 } }
    , { 970,  kInputPin0,             0, rte::kInvalidCmd, { 48,  48,   0,   0,   0 } }
    , { 980,  kInputPin0,             0, rte::kInvalidCmd, { 82,  82,   0,   0,   0 } }
    , { 990,  kInputPin0,             0, rte::kInvalidCmd, {145, 145,   0,   0,   0 } }
    , {1000,  kInputPin0,             0, rte::kInvalidCmd, {250, 250,   0,   0,   0 } }
  };

  classifier_array_type classifiers;
  cal::input_type in;
  size_t nStep;
  log.start("Default_Green_Red.txt");

  // Initialize
  // CalM shall initialize EEPROM from ROM.
  //hal::eeprom::write(cal::eeprom::eManufacturerID, hal::eeprom::kInitial);
  hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
  hal::stubs::millis = aSteps[0].ms;
  hal::stubs::micros = 1000U * hal::stubs::millis;
  
  // Initialize EEPROM
  rte::start();

  // Initialize EEPROM with ROM default values
  rte::ifc_cal_set_defaults::call();
  // And now activate signal kSignalId
  rte::ifc_cal_set_cv::call(cal::cv::eSignalIDBase + kSignalId, kBuiltInSignalIDAusfahrsignal);
  // ... with first output pin kFirstOutputPin
  tmp = cal::signal::make_signal_first_output(cal::signal::values::kOutputType_Onboard, kFirstOutputPin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalFirstOutputBase + kSignalId, tmp);
  // ... with ADC input pin kInputPin0
  tmp = cal::signal::make_signal_input(cal::signal::values::kInputType_ADC, kInputPin0);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputBase + kSignalId, tmp);
  // ... with classifier type kClassifierType
  tmp = kClassifierType;
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputClassifierTypeBase + kSignalId, tmp);

  in.type = cal::input_type::eAdc;
  in.idx = 0;

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
    uint8 target_pos = rte::calm.signals.at(kSignalId).first_target.idx;
    for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
    {
      rte::intensity8_255 pwm;
      rte::ifc_onboard_target_duty_cycles::readElement(target_pos++, pwm);
      log << std::setw(3) << (int)pwm << ", ";
      EXPECT_EQ((uint8)pwm, aSteps[nStep].au8Curs[i]);
    }
    log << std::endl;
  }

  log.stop();
}

// ------------------------------------------------------------------------------------------------
/// Test if aspects and dim ramps are ok for
/// - Commands from classifiers
/// - Start with default (init) aspect
/// - Switch to green
/// - Switch to red
// ------------------------------------------------------------------------------------------------
TEST(Ut_Signal, Default_All)
{
  typedef util::array<uint8, cfg::kNrSignalTargets> signal_target_array;
  using size_type = signal_target_array::size_type;
  using time_type = classifier_array_type::classifier_type::time_type;
  using cmd_type = rte::cmd_type;
  constexpr uint8 kInputPin0 = 54;

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
      {   0,  kInputPin0,             0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , {  10,  kInputPin0,             0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , {  20,  kInputPin0,             0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , {  30,  kInputPin0,             0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , {  40,  kInputPin0,             0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , {  50,  kInputPin0,             0,                4, {  5,   5,   0,   0,   0 } }
    , {  60,  kInputPin0,             0,                4, {  3,   3,   0,   0,   0 } }
    , {  70,  kInputPin0,             0,                4, {  2,   2,   0,   0,   0 } }
    , {  80,  kInputPin0,             0,                4, {  0,   0,   0,   0,   0 } }
    , {  90,  kInputPin0,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 100,  kInputPin0,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 110,  kInputPin0,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 120,  kInputPin0,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 130,  kInputPin0,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 140,  kInputPin0,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 150,  kInputPin0,             0,                4, {  2,   2,   2,   2,   2 } }
    , { 160,  kInputPin0,             0,                4, {  3,   3,   3,   3,   3 } }
    , { 170,  kInputPin0,             0,                4, {  5,   5,   5,   5,   5 } }
    , { 180,  kInputPin0,             0,                4, {  9,   9,   9,   9,   9 } }
    , { 190,  kInputPin0,             0,                4, { 16,  16,  16,  16,  16 } }
    , { 200,  kInputPin0,             0,                4, { 27,  27,  27,  27,  27 } }
    , { 210,  kInputPin0,             0,                4, { 48,  48,  48,  48,  48 } }
    , { 220,  kInputPin0,             0,                4, { 82,  82,  82,  82,  82 } }
    , { 230,  kInputPin0,             0,                4, {145, 145, 145, 145, 145 } }
    , { 240,  kInputPin0,             0,                4, {250, 250, 250, 250, 250 } }
    , { 250,  kInputPin0,             0,                4, {255, 255, 255, 255, 255 } }
  };

  classifier_array_type classifiers;
  cal::input_type in;
  size_t nStep;
  constexpr int kSignalId = 0;
  constexpr int kClassifierId = 0;

  log.start("Default_All.txt");

  // Initialize
  // CalM shall initialize EEPROM from ROM.
  hal::eeprom::write(cal::eeprom::eManufacturerID, hal::eeprom::kInitial);
  hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
  hal::stubs::millis = aSteps[0].ms;
  hal::stubs::micros = 1000U * hal::stubs::millis;

  // CalM initializes EEPROM with ROM values
  rte::start();

  // Manipulate calibration data for classifier kClassifierId (0)
  //const cal::classifier_array_cal_type * pCalCls = rte::ifc_cal_input_classifier::call();
  //cal::classifier_array_element_type cal_cls = pCalCls->classifiers.at(kClassifierId);
  //cal_cls.limits_ptr->lo_limits.at(4) = 0;
  //cal_cls.limits_ptr->hi_limits.at(4) = 255;
  //rte::ifc_cal_set_input_classifier::call(kClassifierId, cal_cls, true);
  rte::ifc_cal_set_cv::call(cal::cv::eClassifierBase + kSignalId, kClassifierId);

  // Prepare RTE call to get command for kClassifierId
  in.type = cal::input_type::eAdc;
  in.idx = kClassifierId;

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    hal::stubs::analogRead[aSteps[nStep].nPin] = aSteps[nStep].nAdc;
    hal::stubs::millis = aSteps[nStep].ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    rte::exec();
    printRte();
    // Get command for kClassifierId
    rte::cmd_type cmd = rte::ifc_rte_get_cmd::call(in);
    log << std::setw(3) << (int)cmd << " ";
    EXPECT_EQ(cmd, aSteps[nStep].cmd);
    uint8 target_pos = rte::calm.signals.at(kSignalId).first_target.idx;
    for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
    {
      // Get pins for current signal target
      rte::intensity8_255 pwm;
      // Read PWM for that pin
      rte::ifc_onboard_target_duty_cycles::readElement(target_pos++, pwm);
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
  //RUN_TEST(Default_All);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
