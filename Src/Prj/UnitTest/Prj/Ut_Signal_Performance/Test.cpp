/**
 * @file Ut_Signal_Performance/Test.cpp
 *
 * @brief Unit tests to measure run time of project Signal
 *
 * @copyright Copyright 2025 Ralf Sondershaus
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
#include <Cal/CalM_config.h>
#include <Hal/EEPROM.h>
#include <Hal/Gpio.h>
#include <Hal/Timer.h>
#include <Rte/Rte.h>
#include <InputClassifier.h>
#include <Util/Array.h>
#include <LedRouter.h>
#include <Dcc/Decoder.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#define PRINT_RTE     0

static constexpr uint8 kBuiltInSignalIDAusfahrsignal = 1;

#include <Cal/CalM_config.h>

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
#if PRINT_RTE == 1
static void printRte()
{
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
}
#endif // PRINT_RTE

// ------------------------------------------------------------------------------------------------
/// 
// ------------------------------------------------------------------------------------------------
TEST(Ut_Signal, InputClassifier1)
{
  signal::InputClassifier classifier;
  constexpr int nrRep = 1000;
  constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
  constexpr uint8 kFirstOutputPin = 13;
  constexpr uint8 kInputPin = 54;
  constexpr uint8 kClassifierType = 0;

  uint32 t1;
  uint32 td;
  uint8 tmp;

  // Initialize EEPROM with ROM default values
  rte::ifc_cal_set_defaults::call();
  // And now activate signal kSignalPos
  rte::ifc_cal_set_cv::call(cal::cv::eSignalIDBase + kSignalPos, kBuiltInSignalIDAusfahrsignal);
  // ... with first output pin kFirstOutputPin
  tmp = cal::values::make_signal_first_output(cal::values::values::kOutputType_Onboard, kFirstOutputPin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalFirstOutputBase + kSignalPos, tmp);
  // ... with ADC input pin kInputPin
  tmp = cal::values::make_signal_input(cal::values::values::kInputType_ADC, kInputPin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputBase + kSignalPos, tmp);
  // ... with classifier type kClassifierType
  tmp = kClassifierType;
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputClassifierTypeBase + kSignalPos, tmp);

  // And now activate signal kSignalPos+1
  rte::ifc_cal_set_cv::call(cal::cv::eSignalIDBase + kSignalPos + 1, kBuiltInSignalIDAusfahrsignal);
  // ... with first output pin kFirstOutputPin+8
  tmp = cal::values::make_signal_first_output(cal::values::values::kOutputType_Onboard, kFirstOutputPin + 8);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalFirstOutputBase + kSignalPos + 1, tmp);
  // ... with ADC input pin kInputPin+1
  tmp = cal::values::make_signal_input(cal::values::values::kInputType_ADC, kInputPin+1);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputBase + kSignalPos + 1, tmp);
  // ... with classifier type kClassifierType
  tmp = kClassifierType;
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputClassifierTypeBase + kSignalPos + 1, tmp);

  t1 = micros();
  for (int i = 0; i < nrRep; i++)
  {
    classifier.init();
  }
  td = micros() - t1;
  hal::serial::print("InputClassifier::init ");
  hal::serial::println(td / nrRep);

  t1 = micros();
  for (int i = 0; i < nrRep; i++)
  {
    hal::stubs::analogRead[kInputPin  ]++;
    hal::stubs::analogRead[kInputPin+1]++;
    classifier.cycle();
  }
  td = micros() - t1;
  hal::serial::print("InputClassifier::cycle ");
  hal::serial::println(td / nrRep);
}

// ------------------------------------------------------------------------------------------------
/// 
// ------------------------------------------------------------------------------------------------
TEST(Ut_Signal, LedRouter_OneRamp)
{
  using target_type = signal::LedRouter::target_type;
  using intensity16_type = signal::LedRouter::intensity16_type;
  using speed16_ms_type = signal::LedRouter::speed16_ms_type;

  signal::LedRouter ledr;
  constexpr int nrRep = 1000;
  target_type tgt;
  const intensity16_type kTgtInt { intensity16_type::kIntensity_100 };
  const speed16_ms_type kTgtSpd { 1 };

  tgt.type = target_type::eOnboard; 
  tgt.idx = 0;

  uint32 t1;
  uint32 td;
  t1 = micros();
  for (int i = 0; i < nrRep; i++)
  {
    ledr.init();
  }
  td = micros() - t1;
  hal::serial::print("LedRouter::init ");
  hal::serial::println(td / nrRep);

  // set target for one ramp
  ledr.setIntensityAndSpeed(tgt, kTgtInt, kTgtSpd);
  t1 = micros();
  for (int i = 0; i < nrRep; i++)
  {
    ledr.cycle();
  }
  td = micros() - t1;
  hal::serial::print("LedRouter::cycle ");
  hal::serial::println(td / nrRep);
}

// ------------------------------------------------------------------------------------------------
/// Speed 1 = 0.000030517578125% / ms, intensity does not reach 100% within 1000 repetitions.
///
/// Output values can be traced via rte::ifc_onboard_target_duty_cycles.
// ------------------------------------------------------------------------------------------------
TEST(Ut_Signal, LedRouter_AllRamps)
{
  using target_type = signal::LedRouter::target_type;
  using intensity16_type = signal::LedRouter::intensity16_type;
  using speed16_ms_type = signal::LedRouter::speed16_ms_type;
  signal::LedRouter ledr;
  constexpr int nrRep = 1000;
  target_type tgt;
  const intensity16_type kTgtInt { intensity16_type::kIntensity_100 };
  const speed16_ms_type kTgtSpd { 1 };
  rte::intensity8_255 intensity8_255;

  tgt.type = target_type::eOnboard; 
  tgt.idx = 0;

  uint32 t1;
  uint32 td;
  t1 = micros();
  for (int i = 0; i < nrRep; i++)
  {
    ledr.init();
  }
  td = micros() - t1;
  hal::serial::print("LedRouter::init ");
  hal::serial::println(td / nrRep);

  // set target for all ramps
  for (tgt.idx = 0; tgt.idx < cfg::kNrOnboardTargets; tgt.idx++)
  {
    ledr.setIntensityAndSpeed(tgt, kTgtInt, kTgtSpd);
  }
  t1 = micros();
  for (int i = 0; i < nrRep; i++)
  {
    ledr.cycle();
  }
  td = micros() - t1;
  hal::serial::print("LedRouter::cycle ");
  hal::serial::println(td / nrRep);
}

namespace dcc 
{
  void ISR_Dcc(void);
}

// ------------------------------------------------------------------------------------------------
///
// ------------------------------------------------------------------------------------------------
TEST(Ut_Signal, ISR_Dcc1)
{
  dcc::Decoder myDec;
  signal::LedRouter ledr;
  constexpr int nrRep = 400; // shall not exceed kTimeBufferSize
  uint32 t1;
  uint32 t2;
  uint32 td;

  td = 0;
  for (int i = 0; i < nrRep; i++)
  {
    t1 = micros();
    dcc::ISR_Dcc();
    t2 = micros() - t1;
    td += t2;
    if (t2 < 92)
    {
      delayMicroseconds(92 - t2); // simulate time between interrupts
    }
  }
  hal::serial::print("ISR_Dcc1 ");
  hal::serial::println(td / nrRep);
  
  // empty the queue for the next run.
  myDec.fetch();
  while (!myDec.empty())
  {
    myDec.pop();
  }
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

  RUN_TEST(InputClassifier1);
  RUN_TEST(LedRouter_OneRamp);
  RUN_TEST(LedRouter_AllRamps);
  RUN_TEST(ISR_Dcc1);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
