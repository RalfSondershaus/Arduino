/**
 * @file Ut_Signal/Test.cpp
 *
 * @brief Unit tests and integration tests for project Signal
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
#include <Dcc/BitExtractor.h>
#include <Dcc/Decoder.h>
#include <Hal/EEPROM.h>
#include <Hal/Gpio.h>
#include <Hal/Timer.h>
#include <Rte/Rte.h>
#include <InputClassifier.h>
#include <Util/Array.h>
#include <Util/String.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#define PRINT_RTE     0

#include <Cal/CalM_config.h>

typedef util::classifier_array<cfg::kNrClassifiers, cfg::kNrClassifierClasses> classifier_array_type;

static constexpr uint8 kBuiltInSignalIDAusfahrsignal = 1;

namespace dcc
{
    /// buffer size. Consider 60 interrupts / 1.5 ms = 180 / 4.5 ms ~ 200 / 5 ms ~ 400 / 10 ms
    static constexpr uint16 kBitStreamSize = 400U;
    /// A FIFO (queue) is used to exchange data between ISR and dcc::Decoder.
    using bit_stream_type = BitStream<kBitStreamSize>;
    /// Share data between ISR and DccDecoder::loop
    /// The buffers for bits are double-buffered to allow
    /// the ISR to write into one buffer while the main loop reads from the other.
    /// The double buffering is implemented with a static index.
    /// The index is toggled in the ISR and in the main loop.
    extern bit_stream_type bit_streams[2];
}

/**
 * @brief 64-character string type.
 * 
 * `11111111111 0 10AAAAAA 0 1AAACDDD 0 EEEEEEEE 1`
 */
using string64 = util::basic_string<64, char>;

/// Helper function: add a valid preamble to a packet
/// Preamble: at least 10 "1" bits followed by a "0" bit
void add_preamble_to_stream(dcc::bit_stream_type& bit_stream)
{
    // Preamble: at least 10 "1" bits followed by a "0" bit
    for (size_t i = 0; i < 11U; i++)
    {
        bit_stream.push(true, false);
    }
    bit_stream.push(false, false);
}
/// Helper function: add all bits of a byte to a packet. Start with MSB, end with LSB.
/// If lastBit is 0, add a "0" bit at the end (for inter-byte separator)
/// If lastBit is 1, add a "1" bit at the end (for end of packet)
void add_byte_to_stream(dcc::bit_stream_type& bit_stream, uint8 byte, uint8 lastBit = 0)
{
    for (size_t i = 0; i < 8U; i++)
    {
        bit_stream.push((byte & static_cast<uint8>(0x80)) ? true : false, false);
        byte <<= 1U;
    }
    bit_stream.push((lastBit == 0) ? false : true, false);
}

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

/**
 * @brief Performs integration testing of signal processing using time-based test sequences
 * 
 * This test function uses state-transition testing technique to verify signal processing
 * by stepping through predefined time sequences and validating outputs against expected values.
 * 
 * It verifies:
 * - Aspect 0 is the default aspect on startup
 * - Signal transitions to aspect 1 (green) when ADC input is equal to green
 * - Signal transitions to aspect 0 (red) when ADC input is equal to red
 * - Correct PWM duty cycles are set for each target during transitions including ramp-up and 
 *   ramp-down
 * 
 * @param signal_pos Position/index of the signal to test
 * @param first_output_pin First PWM output pin number for the signal
 * @param input_pin ADC input pin number to read signal from
 * @param classifier_type Type of classifier to use for signal processing
 * @param log Logger instance for test output
 * 
 * Test sequence:
 * 1. Initializes hardware stubs and RTE
 * 2. Configures signal parameters (ID, I/O pins, classifier)
 * 3. Steps through predefined test sequence validating:
 *    - Command values from RTE
 *    - PWM duty cycles for each target
 * 
 * @note Uses state table driven testing with predefined time steps, inputs and expected outputs
 * @note Test data covers signal startup, classification and PWM ramping scenarios
 */
void do_signal_test_red_green(
    const int signal_pos, 
    uint8 first_output_pin, 
    uint8 input_pin, 
    uint8 classifier_type, 
    Logger& log)
{
  using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
  using size_type = signal_target_array::size_type;
  using time_type = classifier_array_type::classifier_type::time_type;
  using cmd_type = rte::cmd_type;

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
      {   0,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , {  10,  input_pin,             0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , {  20,  input_pin,             0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , {  30,  input_pin,             0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , {  40,  input_pin,             0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , {  50,  input_pin,             0, rte::kInvalidCmd, { 16,  16,   0,   0,   0 } }
    , {  60,  input_pin,             0, rte::kInvalidCmd, { 27,  27,   0,   0,   0 } }
    , {  70,  input_pin,             0, rte::kInvalidCmd, { 48,  48,   0,   0,   0 } }
    , {  80,  input_pin,             0, rte::kInvalidCmd, { 82,  82,   0,   0,   0 } }
    , {  90,  input_pin,             0, rte::kInvalidCmd, {145, 145,   0,   0,   0 } }
    , { 100,  input_pin,             0, rte::kInvalidCmd, {250, 250,   0,   0,   0 } }
    , { 510,  input_pin, cal::kGreenLo, rte::kInvalidCmd, {255, 255,   0,   0,   0 }}
    , { 560,  input_pin, cal::kGreenLo,                1, {145, 145,   0,   0,   0 }}
    , { 570,  input_pin, cal::kGreenLo,                1, { 84,  84,   0,   0,   0 }}
    , { 580,  input_pin, cal::kGreenLo,                1, { 48,  48,   0,   0,   0 }}
    , { 590,  input_pin, cal::kGreenLo,                1, { 28,  28,   0,   0,   0 }}
    , { 600,  input_pin, cal::kGreenLo,                1, { 16,  16,   0,   0,   0 }}
    , { 610,  input_pin, cal::kGreenLo,                1, {  9,   9,   0,   0,   0 }}
    , { 620,  input_pin, cal::kGreenLo,                1, {  5,   5,   0,   0,   0 }}
    , { 630,  input_pin, cal::kGreenLo,                1, {  3,   3,   0,   0,   0 }}
    , { 640,  input_pin, cal::kGreenLo,                1, {  2,   2,   0,   0,   0 }}
    , { 650,  input_pin, cal::kGreenLo,                1, {  0,   0,   0,   0,   0 }}
    , { 660,  input_pin, cal::kGreenLo,                1, {  0,   0,   2,   0,   0 }}
    , { 670,  input_pin, cal::kGreenLo,                1, {  0,   0,   3,   0,   0 }}
    , { 680,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   5,   0,   0 } }
    , { 690,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   9,   0,   0 } }
    , { 700,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  16,   0,   0 } }
    , { 710,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  27,   0,   0 } }
    , { 720,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  48,   0,   0 } }
    , { 730,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  82,   0,   0 } }
    , { 740,  input_pin,             0, rte::kInvalidCmd, {  0,   0, 145,   0,   0 } }
    , { 750,  input_pin,             0, rte::kInvalidCmd, {  0,   0, 250,   0,   0 } }
    , { 760,  input_pin,   cal::kRedLo, rte::kInvalidCmd, {  0,   0, 255,   0,   0 } }
    , { 810,  input_pin,   cal::kRedLo,                0, {  0,   0, 145,   0,   0 } }
    , { 820,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  84,   0,   0 } }
    , { 830,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  48,   0,   0 } }
    , { 840,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  28,   0,   0 } }
    , { 850,  input_pin,             0, rte::kInvalidCmd, {  0,   0,  16,   0,   0 } }
    , { 860,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   9,   0,   0 } }
    , { 870,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   5,   0,   0 } }
    , { 880,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   3,   0,   0 } }
    , { 890,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   2,   0,   0 } }
    , { 900,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , { 910,  input_pin,             0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , { 920,  input_pin,             0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , { 930,  input_pin,             0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , { 940,  input_pin,             0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , { 950,  input_pin,             0, rte::kInvalidCmd, { 16,  16,   0,   0,   0 } }
    , { 960,  input_pin,             0, rte::kInvalidCmd, { 27,  27,   0,   0,   0 } }
    , { 970,  input_pin,             0, rte::kInvalidCmd, { 48,  48,   0,   0,   0 } }
    , { 980,  input_pin,             0, rte::kInvalidCmd, { 82,  82,   0,   0,   0 } }
    , { 990,  input_pin,             0, rte::kInvalidCmd, {145, 145,   0,   0,   0 } }
    , {1000,  input_pin,             0, rte::kInvalidCmd, {250, 250,   0,   0,   0 } }
  };

  classifier_array_type classifiers;
  cal::input_type in;
  size_t nStep;

  // Initialize
  hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
  hal::stubs::millis = aSteps[0].ms;
  hal::stubs::micros = 1000U * hal::stubs::millis;
  
  // Here we go
  rte::start();

  // Initialize EEPROM with ROM default values
  rte::ifc_cal_set_defaults::call();
  // And now activate signal kSignalPos
  rte::ifc_cal_set_cv::call(cal::cv::eSignalIDBase + signal_pos, kBuiltInSignalIDAusfahrsignal);
  // ... with first output pin first_output_pin
  tmp = cal::signal::make_signal_first_output(cal::signal::values::kOutputType_Onboard, first_output_pin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalFirstOutputBase + signal_pos, tmp);
  // ... with ADC input pin input_pin
  tmp = cal::signal::make_signal_input(cal::signal::values::kInputType_ADC, input_pin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputBase + signal_pos, tmp);
  // ... with classifier type classifier_type
  tmp = classifier_type;
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputClassifierTypeBase + signal_pos, tmp);

  in.type = cal::input_type::eAdc;
  in.idx = signal_pos;

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
    uint8 target_pos = rte::calm.signals.at(signal_pos).first_target.idx;
    for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
    {
      rte::intensity8_255 pwm;
      rte::ifc_onboard_target_duty_cycles::readElement(target_pos++, pwm);
      log << std::setw(3) << (int)pwm << ", ";
      EXPECT_EQ((uint8)pwm, aSteps[nStep].au8Curs[i]);
    }
    log << std::endl;
  }
}

/**
 * @test Signal0_ADC_Green_Red
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal0_ADC_Green_Red)
{
  Logger log;
  constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
  constexpr uint8 kFirstOutputPin = 13;
  constexpr uint8 kInputPin = 54;
  constexpr uint8 kClassifierType = 0;

  log.start("Signal0_ADC_Green_Red.txt");

  do_signal_test_red_green(
      kSignalPos, 
      kFirstOutputPin, 
      kInputPin, 
      kClassifierType, 
      log);

  log.stop();
}

/**
 * @test Signal1_ADC_Green_Red
 * @brief Tests whether signal 1 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal1_ADC_Green_Red)
{
  Logger log;
  constexpr int kSignalPos = 1; // Need to configure signal 1 as Ausfahrsignal
  constexpr uint8 kFirstOutputPin = 20;
  constexpr uint8 kInputPin = 55;
  constexpr uint8 kClassifierType = 0;

  log.start("Signal1_ADC_Green_Red.txt");

  do_signal_test_red_green(
      kSignalPos, 
      kFirstOutputPin, 
      kInputPin, 
      kClassifierType, 
      log);

  log.stop();
}

/**
 * @test Signal7_ADC_Green_Red
 * @brief Tests whether signal 7 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal7_ADC_Green_Red)
{
  Logger log;
  constexpr int kSignalPos = 7; // Need to configure signal 7 as Ausfahrsignal
  constexpr uint8 kFirstOutputPin = 20;
  constexpr uint8 kInputPin = 55;
  constexpr uint8 kClassifierType = 0;

  log.start("Signal7_ADC_Green_Red.txt");

  do_signal_test_red_green(
      kSignalPos, 
      kFirstOutputPin, 
      kInputPin, 
      kClassifierType, 
      log);

  log.stop();
}

/**
 * @brief Performs integration testing of signal processing using time-based test sequences
 * 
 * This test function uses state-transition testing technique to verify signal processing
 * by stepping through predefined time sequences and validating outputs against expected values.
 * 
 * It verifies:
 * - Aspect 0 is the default aspect on startup
 * - Signal transitions to aspect 4 (all LEDs on) when ADC input is above green threshold
 * 
 * @param signal_pos Position/index of the signal to test
 * @param first_output_pin First PWM output pin number for the signal
 * @param input_pin ADC input pin number to read signal from
 * @param classifier_type Type of classifier to use for signal processing
 * @param log Logger instance for test output
 * 
 * Test sequence:
 * 1. Initializes hardware stubs and RTE
 * 2. Configures signal parameters (ID, I/O pins, classifier)
 * 3. Steps through predefined test sequence validating:
 *    - Command values from RTE
 *    - PWM duty cycles for each target
 * 
 * @note Uses state table driven testing with predefined time steps, inputs and expected outputs
 * @note Test data covers signal startup, classification and PWM ramping scenarios
 */
void do_signal_test_all(
    const int signal_pos, 
    uint8 first_output_pin, 
    uint8 input_pin, 
    uint8 classifier_type, 
    Logger& log)
{
  using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
  using size_type = signal_target_array::size_type;
  using time_type = classifier_array_type::classifier_type::time_type;
  using cmd_type = rte::cmd_type;

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
      {   0,  input_pin,             0, rte::kInvalidCmd, {  0,   0,   0,   0,   0 } }
    , {  10,  input_pin,             0, rte::kInvalidCmd, {  2,   2,   0,   0,   0 } }
    , {  20,  input_pin,             0, rte::kInvalidCmd, {  3,   3,   0,   0,   0 } }
    , {  30,  input_pin,             0, rte::kInvalidCmd, {  5,   5,   0,   0,   0 } }
    , {  40,  input_pin,             0, rte::kInvalidCmd, {  9,   9,   0,   0,   0 } }
    , {  50,  input_pin,             0,                4, {  5,   5,   0,   0,   0 } }
    , {  60,  input_pin,             0,                4, {  3,   3,   0,   0,   0 } }
    , {  70,  input_pin,             0,                4, {  2,   2,   0,   0,   0 } }
    , {  80,  input_pin,             0,                4, {  0,   0,   0,   0,   0 } }
    , {  90,  input_pin,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 100,  input_pin,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 110,  input_pin,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 120,  input_pin,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 130,  input_pin,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 140,  input_pin,             0,                4, {  0,   0,   0,   0,   0 } }
    , { 150,  input_pin,             0,                4, {  2,   2,   2,   2,   2 } }
    , { 160,  input_pin,             0,                4, {  3,   3,   3,   3,   3 } }
    , { 170,  input_pin,             0,                4, {  5,   5,   5,   5,   5 } }
    , { 180,  input_pin,             0,                4, {  9,   9,   9,   9,   9 } }
    , { 190,  input_pin,             0,                4, { 16,  16,  16,  16,  16 } }
    , { 200,  input_pin,             0,                4, { 27,  27,  27,  27,  27 } }
    , { 210,  input_pin,             0,                4, { 48,  48,  48,  48,  48 } }
    , { 220,  input_pin,             0,                4, { 82,  82,  82,  82,  82 } }
    , { 230,  input_pin,             0,                4, {145, 145, 145, 145, 145 } }
    , { 240,  input_pin,             0,                4, {250, 250, 250, 250, 250 } }
    , { 250,  input_pin,             0,                4, {255, 255, 255, 255, 255 } }
  };

  classifier_array_type classifiers;
  cal::input_type in;
  size_t nStep;
  uint8 tmp;

  // Initialize
  hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
  hal::stubs::millis = aSteps[0].ms;
  hal::stubs::micros = 1000U * hal::stubs::millis;

  // Here we go
  rte::start();

  // Initialize EEPROM with ROM default values (in case another unit test changed 
  // coding data before)
  rte::ifc_cal_set_defaults::call();
  // And now activate signal signal_pos
  rte::ifc_cal_set_cv::call(cal::cv::eSignalIDBase + signal_pos, kBuiltInSignalIDAusfahrsignal);
  // ... with first output pin first_output_pin
  tmp = cal::signal::make_signal_first_output(cal::signal::values::kOutputType_Onboard, first_output_pin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalFirstOutputBase + signal_pos, tmp);
  // ... with ADC input pin input_pin
  tmp = cal::signal::make_signal_input(cal::signal::values::kInputType_ADC, input_pin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputBase + signal_pos, tmp);
  // ... with classifier type classifier_type
  tmp = classifier_type;
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputClassifierTypeBase + signal_pos, tmp);
  // Classifier type 0 shall identify class 4 for AD value 0, so set limits accordingly.
  rte::ifc_cal_set_cv::call(cal::cv::eClassifierType1LoLimitBase + 4, 0);
  rte::ifc_cal_set_cv::call(cal::cv::eClassifierType1HiLimitBase + 4, 255);

  // Prepare RTE call to get command for signal_pos
  in.type = cal::input_type::eAdc;
  in.idx = signal_pos;

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    hal::stubs::analogRead[aSteps[nStep].nPin] = aSteps[nStep].nAdc;
    hal::stubs::millis = aSteps[nStep].ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    rte::exec();
    printRte();
    // Get command for kClassifierPos
    rte::cmd_type cmd = rte::ifc_rte_get_cmd::call(in);
    log << std::setw(3) << (int)cmd << " ";
    EXPECT_EQ(cmd, aSteps[nStep].cmd);
    uint8 target_pos = rte::calm.signals.at(signal_pos).first_target.idx;
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
}

/**
 * @test Signal0_ADC_All
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal0_ADC_All)
{
    constexpr uint8 kFirstOutputPin = 13;
    constexpr uint8 kInputPin = 54;
    constexpr uint8 kClassifierType = 0;
    constexpr int kSignalPos = 0;

    Logger log;

    log.start("Signal0_ADC_All.txt");

    do_signal_test_all(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        log);

    log.stop();
}

/**
 * @test Signal7_ADC_All
 * @brief Tests whether signal 7 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal7_ADC_All)
{
    constexpr uint8 kFirstOutputPin = 13;
    constexpr uint8 kInputPin = 54;
    constexpr uint8 kClassifierType = 0;
    constexpr int kSignalPos = 7;

    Logger log;

    log.start("Signal7_ADC_All.txt");

    do_signal_test_all(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        log);

    log.stop();
}

/**
 * @brief Tests DCC signal aspects 2 and 3 transitions for a railway signal
 * 
 * This test function simulates DCC packet reception and verifies correct signal behavior:
 * - Transitions between aspect 2 and 3
 * - PWM duty cycle changes for signal outputs
 * - Timing of transitions
 * 
 * @param signal_pos Position index of the signal (0-7)
 * @param first_output_pin First pin number for signal outputs
 * @param log Logger reference for test output
 * 
 * @details The test:
 * 1. Initializes signal configuration in EEPROM
 * 2. Simulates DCC packets reception
 * 3. Verifies correct command interpretation
 * 4. Checks PWM duty cycles on signal outputs
 * 5. Validates timing of transitions
 */
void do_signal_dcc_test_aspects_2_3(
    const int signal_pos, 
    uint8 first_output_pin, 
    Logger& log)
{
  using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
  using size_type = signal_target_array::size_type;
  using time_type = classifier_array_type::classifier_type::time_type;
  using cmd_type = rte::cmd_type;

  uint8 tmp;

  typedef struct
  {
    time_type ms; // [ms] current time
    uint8 byte1;
    uint8 byte2;
    cmd_type cmd; // expected value: command on RTE
    signal_target_array au8Curs; // expected value: target onboard duty cycles
  } step_type;

  constexpr time_type kFirstTime_ms = 0; // initial time when RTE is started
  // DCC signal address for byte 0: signal 0 -> 1, signal 1 -> 2, ..., signal 7 -> 8
  const uint8 signal_byte0 = static_cast<uint8>(signal_pos + 1); 
  // byte 0: accessory packet with signal address
  const uint8 byte0 = 0b10000000 | signal_byte0;
  const step_type aSteps[] =
    {
          {  10,  byte0, 0b11110011, 3, {  2,   2,   0,   0,   2 } }
        , {  20,  byte0, 0b11110011, 3, {  3,   3,   0,   0,   3 } }
        , {  30,  byte0, 0b11110011, 3, {  5,   5,   0,   0,   5 } }
        , {  40,  byte0, 0b11110011, 3, {  9,   9,   0,   0,   9 } }
        , {  50,  byte0, 0b11110011, 3, { 16,  16,   0,   0,  16 } }
        , {  60,  byte0, 0b11110011, 3, { 27,  27,   0,   0,  27 } }
        , {  70,  byte0, 0b11110011, 3, { 48,  48,   0,   0,  48 } }
        , {  80,  byte0, 0b11110011, 3, { 82,  82,   0,   0,  82 } }
        , {  90,  byte0, 0b11110011, 3, {145, 145,   0,   0, 145 } }
        , { 100,  byte0, 0b11110011, 3, {250, 250,   0,   0, 250 } }
        , { 110,  byte0, 0b11110011, 3, {255, 255,   0,   0, 255 } }
        , { 120,  byte0, 0b11110010, 2, {145, 145,   0,   0, 145 } }
        , { 130,  byte0, 0b11110010, 2, { 84,  84,   0,   0,  84 } }
        , { 140,  byte0, 0b11110010, 2, { 48,  48,   0,   0,  48 } }
        , { 150,  byte0, 0b11110010, 2, { 28,  28,   0,   0,  28 } }
        , { 160,  byte0, 0b11110010, 2, { 16,  16,   0,   0,  16 } }
        , { 170,  byte0, 0b11110010, 2, {  9,   9,   0,   0,   9 } }
        , { 180,  byte0, 0b11110010, 2, {  5,   5,   0,   0,   5 } }
        , { 190,  byte0, 0b11110010, 2, {  3,   3,   0,   0,   3 } }
        , { 200,  byte0, 0b11110010, 2, {  2,   2,   0,   0,   2 } }
        , { 210,  byte0, 0b11110010, 2, {  0,   0,   0,   0,   0 } }
        , { 220,  byte0, 0b11110010, 2, {  0,   0,   2,   2,   0 } }
        , { 230,  byte0, 0b11110010, 2, {  0,   0,   3,   3,   0 } }
        , { 240,  byte0, 0b11110010, 2, {  0,   0,   5,   5,   0 } }
        , { 250,  byte0, 0b11110010, 2, {  0,   0,   9,   9,   0 } }
        , { 260,  byte0, 0b11110010, 2, {  0,   0,  16,  16,   0 } }
        , { 270,  byte0, 0b11110010, 2, {  0,   0,  27,  27,   0 } }
        , { 280,  byte0, 0b11110010, 2, {  0,   0,  48,  48,   0 } }
        , { 290,  byte0, 0b11110010, 2, {  0,   0,  82,  82,   0 } }
        , { 300,  byte0, 0b11110010, 2, {  0,   0, 145, 145,   0 } }
        , { 310,  byte0, 0b11110010, 2, {  0,   0, 250, 250,   0 } }
        , { 320,  byte0, 0b11110010, 2, {  0,   0, 255, 255,   0 } }
    };
  classifier_array_type classifiers;
  cal::input_type in;
  size_t nStep;

  // Initialize
  hal::stubs::millis = kFirstTime_ms;
  hal::stubs::micros = 1000U * hal::stubs::millis;
  
  // Here we go
  rte::start();

  // Initialize EEPROM with ROM default values
  rte::ifc_cal_set_defaults::call();
  // And now activate signal signal_pos
  rte::ifc_cal_set_cv::call(cal::cv::eSignalIDBase + signal_pos, kBuiltInSignalIDAusfahrsignal);
  // ... with first output pin first_output_pin
  tmp = cal::signal::make_signal_first_output(cal::signal::values::kOutputType_Onboard, first_output_pin);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalFirstOutputBase + signal_pos, tmp);
  // ... with DCC input
  tmp = cal::signal::make_signal_input(cal::signal::values::kInputType_DCC, 0);
  rte::ifc_cal_set_cv::call(cal::cv::eSignalInputBase + signal_pos, tmp);

  in.type = cal::input_type::eDcc;
  in.idx = signal_pos;

  for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
  {
    // Simulate DCC packet received by ISR
    {
      dcc::bit_stream_type& bit_stream = dcc::bit_streams[nStep%2]; // Use alternating bit streams
      add_preamble_to_stream(bit_stream);
      add_byte_to_stream(bit_stream, aSteps[nStep].byte1, 0);
      add_byte_to_stream(bit_stream, aSteps[nStep].byte2, 0);
      add_byte_to_stream(bit_stream, aSteps[nStep].byte1 ^ aSteps[nStep].byte2, 1);
    }
    hal::stubs::millis = aSteps[nStep].ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    rte::exec();
    printRte();
    rte::cmd_type cmd = rte::ifc_rte_get_cmd::call(in);
    log << std::setw(3) << (int)cmd << " - ";
    EXPECT_EQ(cmd, aSteps[nStep].cmd);
    uint8 target_pos = rte::calm.signals.at(signal_pos).first_target.idx;
    for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
    {
      rte::intensity8_255 pwm;
      rte::ifc_onboard_target_duty_cycles::readElement(target_pos++, pwm);
      log << std::setw(3) << (int)pwm << ", ";
      EXPECT_EQ((uint8)pwm, aSteps[nStep].au8Curs[i]);
    }
    log << std::endl;
  }
}

/**
 * @test Signal0_DCC_Aspects_2_3
 * @brief Tests whether signal 0 is correctly triggered by DCC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal0_DCC_Aspects_2_3)
{
    Logger log;
    constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 13;

    log.start("Signal0_DCC_Aspects_2_3.txt");

    do_signal_dcc_test_aspects_2_3(
        kSignalPos,
        kFirstOutputPin,
        log);

    log.stop();
}

/**
 * @test Signal7_DCC_Aspects_2_3
 * @brief Tests whether signal 7 is correctly triggered by DCC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal7_DCC_Aspects_2_3)
{
    Logger log;
    constexpr int kSignalPos = 7; // Need to configure signal 7 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 13;

    log.start("Signal7_DCC_Aspects_2_3.txt");

    do_signal_dcc_test_aspects_2_3(
        kSignalPos,
        kFirstOutputPin,
        log);

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

  RUN_TEST(Signal0_ADC_Green_Red);
  RUN_TEST(Signal1_ADC_Green_Red);
  RUN_TEST(Signal7_ADC_Green_Red);
  RUN_TEST(Signal0_ADC_All);
  RUN_TEST(Signal7_ADC_All);
  RUN_TEST(Signal0_DCC_Aspects_2_3);
  RUN_TEST(Signal7_DCC_Aspects_2_3);

  (void) UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
