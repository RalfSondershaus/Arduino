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
#include <Rte/Rte_Cfg_Cod.h>
#include <InputClassifier.h>
#include <Util/Array.h>
#include <Util/String.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#define PRINT_RTE 0

#include <Cal/CalM_config.h>

/**
 * @brief Expected values for built-in signal aspects
 */
#define CAL_BUILT_IN_SIGNAL_OUTPUTS     \
    {                                   \
        5,                              \
        0b00011000, 0b00000000,         \
        0b00000100, 0b00000000,         \
        0b00000110, 0b00000000,         \
        0b00011001, 0b00000000,         \
        0b00011111, 0b00000000,         \
        0b00011111, 0b00000000,         \
        0b00011111, 0b00000000,         \
        0b00011111, 0b00000000,         \
        10, 10,                         \
        /* Blocksignal */               \
        /* red green */                 \
        2,                              \
        0b00000010, 0b00000000,         \
        0b00000001, 0b00000000,         \
        0b00000001, 0b00000000,         \
        0b00000010, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        10, 10,                         \
        /* Einfahrsignal */             \
        /* red red green yellow */      \
        4,                              \
        0b00001100, 0b00000000,         \
        0b00000010, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00001100, 0b00000000,         \
        0b00001111, 0b00000000,         \
        0b00001111, 0b00000000,         \
        0b00001111, 0b00000000,         \
        0b00001111, 0b00000000,         \
        10, 10                          \
    }

typedef util::classifier_array<cfg::kNrClassifiers, cfg::kNrClassifierClasses> classifier_array_type;

static constexpr uint8 kBuiltInSignalIDAusfahrsignal = 1;
static constexpr uint8 kBuiltInSignalIDEinfahrsignal = 3;

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
void add_preamble_to_stream(dcc::bit_stream_type &bit_stream)
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
void add_byte_to_stream(dcc::bit_stream_type &bit_stream, uint8 byte, uint8 lastBit = 0)
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
 * @test CalM_get_signal_id
 * @brief Tests whether signal configuration CVs can be set and read correctly
 *        and whether the signal ID, first output and input pin are returned correctly.
 */
TEST(Ut_Signal, CalM_get_signal_id)
{
    uint8 signal_pos = 0;
    uint8 first_output_pin = 13;
    uint8 input_pin = 54;
    uint8 classifier_type = 0;
    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    for (size_t i = 0; i < rte::calm.eeprom_data_buffer.size(); i++)
    {
        EXPECT_EQ(hal::eeprom::read(static_cast<int>(i)), rte::calm.eeprom_data_buffer[i]);
    }
    // And now activate signal kSignalPos
    rte::set_cv(cal::cv::kSignalIDBase + signal_pos, kBuiltInSignalIDAusfahrsignal);
    EXPECT_EQ(rte::get_cv(cal::cv::kSignalIDBase + signal_pos), kBuiltInSignalIDAusfahrsignal);
    EXPECT_EQ(rte::sig::get_signal_id(signal_pos), kBuiltInSignalIDAusfahrsignal);
    // ... with first output pin first_output_pin
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_pos, cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin));
    EXPECT_EQ(rte::sig::get_first_output(signal_pos).pin, first_output_pin);
    EXPECT_EQ(rte::sig::get_first_output(signal_pos).type, cal::constants::kOnboard);
    // ... with ADC input pin input_pin
    rte::set_cv(cal::cv::kSignalInputBase + signal_pos, cal::constants::make_signal_input(cal::constants::kAdc, input_pin));
    EXPECT_EQ(rte::get_cv(cal::cv::kSignalInputBase + signal_pos), static_cast<uint8>(cal::constants::kAdc << 6 | (input_pin & 0x3F)));
    EXPECT_EQ(rte::sig::get_input(signal_pos).type, cal::constants::kAdc);
    EXPECT_EQ(rte::sig::get_input(signal_pos).pin, input_pin);
    // ... with classifier type classifier_type
    rte::set_cv(cal::cv::kSignalInputClassifierTypeBase + signal_pos, classifier_type);
    EXPECT_EQ(util::classifier_cal::get_classifier_type(signal_pos), classifier_type);
    // ... with inverse output pin order
    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_pos, 1U); // set inverse order bit
    EXPECT_EQ(rte::sig::is_output_pin_order_inverse(signal_pos), true);
    // .. with step size 2
    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_pos, 0b00000010); // set step size to 2
    EXPECT_EQ(rte::sig::get_output_pin_step_size(signal_pos), static_cast<uint8>(2U));
#if 0
  Logger log;
  constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
  constexpr uint8 kFirstOutputPin = 13;
  constexpr uint8 kInputPin = 54;
  constexpr uint8 kClassifierType = 0;

#endif
}

/**
 * @test CalM_update_cv_id
 * @brief Tests whether updating a CV causes the new value to be written to EEPROM.
 *        This test initializes the EEPROM with default values, sets a specific CV,
 *        and verifies that the EEPROM contains the updated value.
 */
TEST(Ut_Signal, CalM_update_cv_id)
{
    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    for (size_t i = 0; i < rte::calm.eeprom_data_buffer.size(); i++)
    {
        EXPECT_EQ(hal::eeprom::read(static_cast<int>(i)), rte::calm.eeprom_data_buffer[i]);
    }
    // Now set CV for signal ID and verify EEPROM is updated
    rte::set_cv(cal::cv::kSignalIDBase + 0, kBuiltInSignalIDAusfahrsignal);
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalIDBase + 0), kBuiltInSignalIDAusfahrsignal);
}

/**
 * @test CalM_is_output_pin_step_size_1
 * @brief Tests whether updating a CV for output pin configuration to step size 1 causes the new 
 *        value to be written to EEPROM and interpreted correctly.
 *        This test initializes the EEPROM with default values, sets a specific CV,
 *        verifies that the EEPROM contains the updated value, and verifies that the output pins
 *        are correctly identified.
 */
TEST(Ut_Signal, CalM_is_output_pin_step_size_1)
{
    const uint8 first_output_pin = 13;
    const uint8 first_output = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    const uint8 signal_idx = 0;
    const uint8 expected_num_targets = 5;
    const uint8 cmd = 0; // helper variable to call get_signal_aspect
    struct signal::signal_aspect signal_asp;

    const uint8 signal_id = rte::sig::get_signal_id(signal_idx);

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // Now set CV for signal ID and verify EEPROM is updated
    rte::set_cv(cal::cv::kSignalIDBase + signal_idx, kBuiltInSignalIDAusfahrsignal);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_idx, first_output);
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalFirstOutputBase + signal_idx), first_output);
    EXPECT_EQ(rte::sig::get_first_output(signal_idx).pin, first_output_pin);
    rte::sig::get_signal_aspect(signal_id, cmd, signal_asp);
    EXPECT_EQ(signal_asp.num_targets, expected_num_targets);
    for (size_t i = 0U; i < expected_num_targets; i++)
    {
        EXPECT_EQ(rte::sig::is_output_pin(first_output_pin + i), true);
    }
}

/**
 * @test CalM_is_output_pin_step_size_2
 * @brief Tests whether updating a CV for output pin configuration to step size 2 causes the new 
 *        value to be written to EEPROM and interpreted correctly.
 *        This test initializes the EEPROM with default values, sets a specific CV,
 *        verifies that the EEPROM contains the updated value, and verifies that the output pins
 *        are correctly identified.
 */
TEST(Ut_Signal, CalM_is_output_pin_step_size_2)
{
    const uint8 first_output_pin = 13;
    const uint8 first_output = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    const uint8 signal_idx = 0;
    const uint8 expected_num_targets = 5;
    const uint8 cmd = 0; // helper variable to call get_signal_aspect
    struct signal::signal_aspect signal_asp;

    const uint8 signal_id = rte::sig::get_signal_id(signal_idx);

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // Now set CVs for signal ID, first output, and output config and verify EEPROM is updated
    rte::set_cv(cal::cv::kSignalIDBase + signal_idx, kBuiltInSignalIDAusfahrsignal);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_idx, first_output);
    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_idx, 0b00000010); // set step size to 2
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalFirstOutputBase + signal_idx), first_output);
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalOutputConfigBase + signal_idx), static_cast<uint8>(0b00000010));
    EXPECT_EQ(rte::sig::get_first_output(signal_idx).pin, first_output_pin);
    const uint8 step_size = rte::sig::get_output_pin_step_size(signal_idx);
    EXPECT_EQ(step_size, static_cast<uint8>(2U));
    rte::sig::get_signal_aspect(signal_id, cmd, signal_asp);
    EXPECT_EQ(signal_asp.num_targets, expected_num_targets);
    for (size_t i = 0U; i < expected_num_targets; i++)
    {
        EXPECT_EQ(rte::sig::is_output_pin(static_cast<uint8>(first_output_pin + i * step_size)), true);
    }
}

/**
 * @test CalM_is_output_pin_step_size_m1
 * @brief Tests whether updating a CV for output pin configuration to step size -1 causes the new 
 *        value to be written to EEPROM and interpreted correctly.
 *        This test initializes the EEPROM with default values, sets a specific CV,
 *        verifies that the EEPROM contains the updated value, and verifies that the output pins
 *        are correctly identified.
 */
TEST(Ut_Signal, CalM_is_output_pin_step_size_m1)
{
    const uint8 first_output_pin = 13;
    const uint8 first_output = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    const uint8 signal_idx = 0;
    const uint8 expected_num_targets = 5;
    const uint8 cmd = 0; // helper variable to call get_signal_aspect
    struct signal::signal_aspect signal_asp;

    const uint8 signal_id = rte::sig::get_signal_id(signal_idx);

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // Now set CVs for signal ID, first output, and output config and verify EEPROM is updated
    rte::set_cv(cal::cv::kSignalIDBase + signal_idx, kBuiltInSignalIDAusfahrsignal);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_idx, first_output);
    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_idx, 0b00000001); // set inverse order
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalFirstOutputBase + signal_idx), first_output);
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalOutputConfigBase + signal_idx), static_cast<uint8>(0b00000001));
    EXPECT_EQ(rte::sig::get_first_output(signal_idx).pin, first_output_pin);
    rte::sig::get_signal_aspect(signal_id, cmd, signal_asp);
    EXPECT_EQ(signal_asp.num_targets, expected_num_targets);
    // adjust pin increment according to pin order configuration
    const sint8 pin_inc = rte::sig::is_output_pin_order_inverse(signal_idx) ? 
                         -rte::sig::get_output_pin_step_size(signal_idx) : 
                          rte::sig::get_output_pin_step_size(signal_idx);
    EXPECT_EQ(pin_inc, static_cast<sint8>(-1));
    for (size_t i = 0U; i < expected_num_targets; i++)
    {
        EXPECT_EQ(rte::sig::is_output_pin(static_cast<uint8>(first_output_pin + i * pin_inc)), true);
    }
}

/**
 * @test CalM_is_output_pin_step_size_m2
 * @brief Tests whether updating a CV for output pin configuration to step size -2 causes the new 
 *        value to be written to EEPROM and interpreted correctly.
 *        This test initializes the EEPROM with default values, sets a specific CV,
 *        verifies that the EEPROM contains the updated value, and verifies that the output pins
 *        are correctly identified.
 */
TEST(Ut_Signal, CalM_is_output_pin_step_size_m2)
{
    const uint8 first_output_pin = 13;
    const uint8 first_output = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    const uint8 signal_idx = 0;
    const uint8 expected_num_targets = 5;
    const uint8 cmd = 0; // helper variable to call get_signal_aspect
    struct signal::signal_aspect signal_asp;

    const uint8 signal_id = rte::sig::get_signal_id(signal_idx);

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // Now set CVs for signal ID, first output, and output config and verify EEPROM is updated
    rte::set_cv(cal::cv::kSignalIDBase + signal_idx, kBuiltInSignalIDAusfahrsignal);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_idx, first_output);
    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_idx, 0b00000011); // set step size to 2 and inverse order
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalFirstOutputBase + signal_idx), first_output);
    EXPECT_EQ(hal::eeprom::read(cal::cv::kSignalOutputConfigBase + signal_idx), static_cast<uint8>(0b00000011));
    EXPECT_EQ(rte::sig::get_first_output(signal_idx).pin, first_output_pin);
    rte::sig::get_signal_aspect(signal_id, cmd, signal_asp);
    EXPECT_EQ(signal_asp.num_targets, expected_num_targets);
    // adjust pin increment according to pin order configuration
    const sint8 pin_inc = rte::sig::is_output_pin_order_inverse(signal_idx) ? 
                         -rte::sig::get_output_pin_step_size(signal_idx) : 
                          rte::sig::get_output_pin_step_size(signal_idx);
    EXPECT_EQ(pin_inc, static_cast<sint8>(-2));
    for (size_t i = 0U; i < expected_num_targets; i++)
    {
        EXPECT_EQ(rte::sig::is_output_pin(static_cast<uint8>(first_output_pin + i * pin_inc)), true);
    }
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
    sint8 step_size,
    Logger &log)
{
    using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
    using size_type = signal_target_array::size_type;
    using time_type = classifier_array_type::classifier_type::time_type;

    uint8 tmp;

    typedef struct
    {
        time_type ms;                // [ms] current time
        int nPin;                    // input pin for AD value
        int nAdc;                    // current AD value for pin
        uint8 cmd;                   // expected value: command on RTE
        signal_target_array au8Curs; // expected value: target onboard duty cycles
    } step_type;

    const step_type aSteps[] =
        {
            {0, input_pin, 0, signal::kInvalidCmd, {0, 0, 0, 0, 0}}, {10, input_pin, 0, signal::kInvalidCmd, {2, 2, 0, 0, 0}}, {20, input_pin, 0, signal::kInvalidCmd, {3, 3, 0, 0, 0}}, {30, input_pin, 0, signal::kInvalidCmd, {5, 5, 0, 0, 0}}, {40, input_pin, 0, signal::kInvalidCmd, {9, 9, 0, 0, 0}}, {50, input_pin, 0, signal::kInvalidCmd, {16, 16, 0, 0, 0}}, {60, input_pin, 0, signal::kInvalidCmd, {27, 27, 0, 0, 0}}, {70, input_pin, 0, signal::kInvalidCmd, {48, 48, 0, 0, 0}}, {80, input_pin, 0, signal::kInvalidCmd, {82, 82, 0, 0, 0}}, {90, input_pin, 0, signal::kInvalidCmd, {145, 145, 0, 0, 0}}, {100, input_pin, 0, signal::kInvalidCmd, {250, 250, 0, 0, 0}}, {510, input_pin, cal::kGreenLo, signal::kInvalidCmd, {255, 255, 0, 0, 0}}, {560, input_pin, cal::kGreenLo, 1, {145, 145, 0, 0, 0}}, {570, input_pin, cal::kGreenLo, 1, {84, 84, 0, 0, 0}}, {580, input_pin, cal::kGreenLo, 1, {48, 48, 0, 0, 0}}, {590, input_pin, cal::kGreenLo, 1, {28, 28, 0, 0, 0}}, {600, input_pin, cal::kGreenLo, 1, {16, 16, 0, 0, 0}}, {610, input_pin, cal::kGreenLo, 1, {9, 9, 0, 0, 0}}, {620, input_pin, cal::kGreenLo, 1, {5, 5, 0, 0, 0}}, {630, input_pin, cal::kGreenLo, 1, {3, 3, 0, 0, 0}}, {640, input_pin, cal::kGreenLo, 1, {2, 2, 0, 0, 0}}, {650, input_pin, cal::kGreenLo, 1, {0, 0, 0, 0, 0}}, {660, input_pin, cal::kGreenLo, 1, {0, 0, 2, 0, 0}}, {670, input_pin, cal::kGreenLo, 1, {0, 0, 3, 0, 0}}, {680, input_pin, 0, signal::kInvalidCmd, {0, 0, 5, 0, 0}}, {690, input_pin, 0, signal::kInvalidCmd, {0, 0, 9, 0, 0}}, {700, input_pin, 0, signal::kInvalidCmd, {0, 0, 16, 0, 0}}, {710, input_pin, 0, signal::kInvalidCmd, {0, 0, 27, 0, 0}}, {720, input_pin, 0, signal::kInvalidCmd, {0, 0, 48, 0, 0}}, {730, input_pin, 0, signal::kInvalidCmd, {0, 0, 82, 0, 0}}, {740, input_pin, 0, signal::kInvalidCmd, {0, 0, 145, 0, 0}}, {750, input_pin, 0, signal::kInvalidCmd, {0, 0, 250, 0, 0}}, {760, input_pin, cal::kRedLo, signal::kInvalidCmd, {0, 0, 255, 0, 0}}, {810, input_pin, cal::kRedLo, 0, {0, 0, 145, 0, 0}}, {820, input_pin, 0, signal::kInvalidCmd, {0, 0, 84, 0, 0}}, {830, input_pin, 0, signal::kInvalidCmd, {0, 0, 48, 0, 0}}, {840, input_pin, 0, signal::kInvalidCmd, {0, 0, 28, 0, 0}}, {850, input_pin, 0, signal::kInvalidCmd, {0, 0, 16, 0, 0}}, {860, input_pin, 0, signal::kInvalidCmd, {0, 0, 9, 0, 0}}, {870, input_pin, 0, signal::kInvalidCmd, {0, 0, 5, 0, 0}}, {880, input_pin, 0, signal::kInvalidCmd, {0, 0, 3, 0, 0}}, {890, input_pin, 0, signal::kInvalidCmd, {0, 0, 2, 0, 0}}, {900, input_pin, 0, signal::kInvalidCmd, {0, 0, 0, 0, 0}}, {910, input_pin, 0, signal::kInvalidCmd, {2, 2, 0, 0, 0}}, {920, input_pin, 0, signal::kInvalidCmd, {3, 3, 0, 0, 0}}, {930, input_pin, 0, signal::kInvalidCmd, {5, 5, 0, 0, 0}}, {940, input_pin, 0, signal::kInvalidCmd, {9, 9, 0, 0, 0}}, {950, input_pin, 0, signal::kInvalidCmd, {16, 16, 0, 0, 0}}, {960, input_pin, 0, signal::kInvalidCmd, {27, 27, 0, 0, 0}}, {970, input_pin, 0, signal::kInvalidCmd, {48, 48, 0, 0, 0}}, {980, input_pin, 0, signal::kInvalidCmd, {82, 82, 0, 0, 0}}, {990, input_pin, 0, signal::kInvalidCmd, {145, 145, 0, 0, 0}}, {1000, input_pin, 0, signal::kInvalidCmd, {250, 250, 0, 0, 0}}};

    classifier_array_type classifiers;
    struct signal::input_cmd in;
    size_t nStep;

    // Initialize
    hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
    hal::stubs::millis = aSteps[0].ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    hal::init_gpio();

    // Here we go
    rte::start();

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // And now activate signal kSignalPos
    rte::set_cv(cal::cv::kSignalIDBase + signal_pos, kBuiltInSignalIDAusfahrsignal);
    // ... with first output pin first_output_pin
    tmp = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_pos, tmp);
    // ... with ADC input pin input_pin
    tmp = cal::constants::make_signal_input(cal::constants::kAdc, input_pin);
    rte::set_cv(cal::cv::kSignalInputBase + signal_pos, tmp);
    // ... with classifier type classifier_type
    tmp = classifier_type;
    rte::set_cv(cal::cv::kSignalInputClassifierTypeBase + signal_pos, tmp);
    // ... with inverse output pin order and/or step size
    tmp = 0U;
    if (step_size < 0)
    {
        tmp = 0b00000001U; // set inverse order bit
    }
    // ... with step size 2
    if ((step_size == -2) || (step_size == 2))
    {
        tmp |= 0b00000010U; // set step size to 2
    } 
    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_pos, tmp);
    in.type = cal::constants::kAdc;
    in.idx = signal_pos;

    for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
    {
        hal::stubs::analogRead[aSteps[nStep].nPin] = aSteps[nStep].nAdc;
        hal::stubs::millis = aSteps[nStep].ms;
        hal::stubs::micros = 1000U * hal::stubs::millis;
        rte::exec();
        printRte();
        uint8 cmd = rte::ifc_rte_get_cmd::call(in);
        log << std::setw(3) << (int)cmd << " ";
        EXPECT_EQ(cmd, aSteps[nStep].cmd);
        uint8 target_pin = cal::constants::extract_signal_first_output_pin(rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_pos));
        for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
        {
            // Get pins for current signal target
            util::intensity8_255 pwm_rte;
            util::intensity8_255 pwm_hal;
            // Read PWM for that pin
            rte::ifc_onboard_target_duty_cycles::readElement(target_pin, pwm_rte);
            pwm_hal = hal::stubs::analogWrite[target_pin];
            target_pin = static_cast<uint8>(target_pin + step_size);
            log << std::setw(3) << (int)pwm_rte << ", ";
            EXPECT_EQ((uint8)pwm_rte, aSteps[nStep].au8Curs[i]);
            EXPECT_EQ((uint8)pwm_hal, aSteps[nStep].au8Curs[i]);
        }
        log << std::endl;
    }
}

/**
 * @test Signal0_ADC_Green_Red_StepSize_1
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size 1 for output pins.
 */
TEST(Ut_Signal, Signal0_ADC_Green_Red_StepSize_1)
{
    Logger log;
    constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 13;
    constexpr uint8 kInputPin = 54;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = 1;

    log.start("Signal0_ADC_Green_Red_StepSize_1.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal1_ADC_Green_Red_StepSize_1
 * @brief Tests whether signal 1 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size 1 for output pins.
 */
TEST(Ut_Signal, Signal1_ADC_Green_Red_StepSize_1)
{
    Logger log;
    constexpr int kSignalPos = 1; // Need to configure signal 1 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = 1;

    log.start("Signal1_ADC_Green_Red_StepSize_1.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal7_ADC_Green_Red_StepSize_1
 * @brief Tests whether signal 7 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size 1 for output pins.
 */
TEST(Ut_Signal, Signal7_ADC_Green_Red_StepSize_1)
{
    Logger log;
    constexpr int kSignalPos = 7; // Need to configure signal 7 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = 1;

    log.start("Signal7_ADC_Green_Red_StepSize_1.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal0_ADC_Green_Red_StepSize_2
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size 2 for output pins.
 */
TEST(Ut_Signal, Signal0_ADC_Green_Red_StepSize_2)
{
    Logger log;
    constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 13;
    constexpr uint8 kInputPin = 54;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = 2;

    log.start("Signal0_ADC_Green_Red_StepSize_2.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal1_ADC_Green_Red_StepSize_2
 * @brief Tests whether signal 1 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size 2 for output pins.
 */
TEST(Ut_Signal, Signal1_ADC_Green_Red_StepSize_2)
{
    Logger log;
    constexpr int kSignalPos = 1; // Need to configure signal 1 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = 2;

    log.start("Signal1_ADC_Green_Red_StepSize_2.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal7_ADC_Green_Red_StepSize_2
 * @brief Tests whether signal 1 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *       Uses step size 2 for output pins.
 */
TEST(Ut_Signal, Signal7_ADC_Green_Red_StepSize_2)
{
    Logger log;
    constexpr int kSignalPos = 7; // Need to configure signal 7 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = 2;

    log.start("Signal7_ADC_Green_Red_StepSize_2.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal0_ADC_Green_Red_StepSize_m1
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size -1 for output pins.
 */
TEST(Ut_Signal, Signal0_ADC_Green_Red_StepSize_m1)
{
    Logger log;
    constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 13;
    constexpr uint8 kInputPin = 54;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = -1;

    log.start("Signal0_ADC_Green_Red_StepSize_m1.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal1_ADC_Green_Red_StepSize_m1
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size -2 for output pins.
 */
TEST(Ut_Signal, Signal1_ADC_Green_Red_StepSize_m1)
{
    Logger log;
    constexpr int kSignalPos = 1; // Need to configure signal 1 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = -1;

    log.start("Signal1_ADC_Green_Red_StepSize_m1.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal7_ADC_Green_Red_StepSize_m1
 * @brief Tests whether signal 7 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size -2 for output pins.
 */
TEST(Ut_Signal, Signal7_ADC_Green_Red_StepSize_m1)
{
    Logger log;
    constexpr int kSignalPos = 7; // Need to configure signal 7 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = -1;

    log.start("Signal1_ADC_Green_Red_StepSize_m1.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal0_ADC_Green_Red_StepSize_m2
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size -2 for output pins.
 */
TEST(Ut_Signal, Signal0_ADC_Green_Red_StepSize_m2)
{
    Logger log;
    constexpr int kSignalPos = 0; // Need to configure signal 0 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 13;
    constexpr uint8 kInputPin = 54;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = -2;

    log.start("Signal0_ADC_Green_Red_StepSize_m2.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal1_ADC_Green_Red_StepSize_m2
 * @brief Tests whether signal 1 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size -2 for output pins.
 */
TEST(Ut_Signal, Signal1_ADC_Green_Red_StepSize_m2)
{
    Logger log;
    constexpr int kSignalPos = 1; // Need to configure signal 1 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = -2;

    log.start("Signal1_ADC_Green_Red_StepSize_m2.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @test Signal7_ADC_Green_Red_StepSize_m2
 * @brief Tests whether signal 7 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size -2 for output pins.
 */
TEST(Ut_Signal, Signal7_ADC_Green_Red_StepSize_m2)
{
    Logger log;
    constexpr int kSignalPos = 7; // Need to configure signal 7 as Ausfahrsignal
    constexpr uint8 kFirstOutputPin = 20;
    constexpr uint8 kInputPin = 55;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = -2;

    log.start("Signal7_ADC_Green_Red_StepSize_m2.txt");

    do_signal_test_red_green(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
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
    Logger &log)
{
    using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
    using size_type = signal_target_array::size_type;
    using time_type = classifier_array_type::classifier_type::time_type;

    typedef struct
    {
        time_type ms;                // [ms] current time
        int nPin;                    // input pin for AD value
        int nAdc;                    // current AD value for pin
        uint8 cmd;                   // expected value: command on RTE
        signal_target_array au8Curs; // expected value: target onboard duty cycles
    } step_type;

    const step_type aSteps[] =
        {
            {0, input_pin, 0, signal::kInvalidCmd, {0, 0, 0, 0, 0}}, 
            {10, input_pin, 0, signal::kInvalidCmd, {2, 2, 0, 0, 0}}, 
            {20, input_pin, 0, signal::kInvalidCmd, {3, 3, 0, 0, 0}}, 
            {30, input_pin, 0, signal::kInvalidCmd, {5, 5, 0, 0, 0}}, 
            {40, input_pin, 0, signal::kInvalidCmd, {9, 9, 0, 0, 0}}, 
            {50, input_pin, 0, 4, {5, 5, 0, 0, 0}}, 
            {60, input_pin, 0, 4, {3, 3, 0, 0, 0}}, 
            {70, input_pin, 0, 4, {2, 2, 0, 0, 0}}, 
            {80, input_pin, 0, 4, {0, 0, 0, 0, 0}}, 
            {90, input_pin, 0, 4, {0, 0, 0, 0, 0}}, 
            {100, input_pin, 0, 4, {0, 0, 0, 0, 0}}, 
            {110, input_pin, 0, 4, {0, 0, 0, 0, 0}}, 
            {120, input_pin, 0, 4, {0, 0, 0, 0, 0}}, 
            {130, input_pin, 0, 4, {0, 0, 0, 0, 0}}, 
            {140, input_pin, 0, 4, {0, 0, 0, 0, 0}}, 
            {150, input_pin, 0, 4, {2, 2, 2, 2, 2}}, 
            {160, input_pin, 0, 4, {3, 3, 3, 3, 3}}, 
            {170, input_pin, 0, 4, {5, 5, 5, 5, 5}}, 
            {180, input_pin, 0, 4, {9, 9, 9, 9, 9}}, 
            {190, input_pin, 0, 4, {16, 16, 16, 16, 16}}, 
            {200, input_pin, 0, 4, {27, 27, 27, 27, 27}}, 
            {210, input_pin, 0, 4, {48, 48, 48, 48, 48}}, 
            {220, input_pin, 0, 4, {82, 82, 82, 82, 82}}, 
            {230, input_pin, 0, 4, {145, 145, 145, 145, 145}}, 
            {240, input_pin, 0, 4, {250, 250, 250, 250, 250}}, 
            {250, input_pin, 0, 4, {255, 255, 255, 255, 255}}};

    classifier_array_type classifiers;
    struct signal::input_cmd in;
    size_t nStep;
    uint8 tmp;

    // Initialize
    hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
    hal::stubs::millis = aSteps[0].ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    hal::init_gpio();

    // Here we go
    rte::start();

    // Initialize EEPROM with ROM default values (in case another unit test changed
    // coding data before)
    rte::ifc_cal_set_defaults();
    // And now activate signal signal_pos
    rte::set_cv(cal::cv::kSignalIDBase + signal_pos, kBuiltInSignalIDAusfahrsignal);
    // ... with first output pin first_output_pin
    tmp = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_pos, tmp);
    // ... with ADC input pin input_pin
    tmp = cal::constants::make_signal_input(cal::constants::kAdc, input_pin);
    rte::set_cv(cal::cv::kSignalInputBase + signal_pos, tmp);
    // ... with classifier type classifier_type
    tmp = classifier_type;
    rte::set_cv(cal::cv::kSignalInputClassifierTypeBase + signal_pos, tmp);
    // Classifier type 0 shall identify class 4 for AD value 0, so set limits accordingly.
    // + 1 to skip debounce time
    // + 4 for class 4
    rte::set_cv(cal::cv::kClassifierBase + 1 + 4, 0);
    rte::set_cv(cal::cv::kClassifierBase + 1 + 4 + cfg::kNrClassifierClasses, 255);

    // Prepare RTE call to get command for signal_pos
    in.type = cal::constants::kAdc;
    in.idx = signal_pos;

    for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
    {
        hal::stubs::analogRead[aSteps[nStep].nPin] = aSteps[nStep].nAdc;
        hal::stubs::millis = aSteps[nStep].ms;
        hal::stubs::micros = 1000U * hal::stubs::millis;
        rte::exec();
        printRte();
        // Get command for kClassifierPos
        uint8 cmd = rte::ifc_rte_get_cmd::call(in);
        log << std::setw(3) << (int)cmd << " ";
        EXPECT_EQ(cmd, aSteps[nStep].cmd);
        uint8 target_pin = cal::constants::extract_signal_first_output_pin(rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_pos));
        for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
        {
            // Get pins for current signal target
            util::intensity8_255 pwm_rte;
            util::intensity8_255 pwm_hal;
            // Read PWM for that pin
            rte::ifc_onboard_target_duty_cycles::readElement(target_pin, pwm_rte);
            pwm_hal = hal::stubs::analogWrite[target_pin++];
            log << std::setw(3) << (int)pwm_rte << ", ";
            EXPECT_EQ((uint8)pwm_rte, aSteps[nStep].au8Curs[i]);
            EXPECT_EQ((uint8)pwm_hal, aSteps[nStep].au8Curs[i]);
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
    Logger &log)
{
    using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
    using size_type = signal_target_array::size_type;
    using time_type = classifier_array_type::classifier_type::time_type;

    uint8 tmp;

    typedef struct
    {
        time_type ms; // [ms] current time
        uint8 byte1;
        uint8 byte2;
        uint8 cmd;                   // expected value: command on RTE
        signal_target_array au8Curs; // expected value: target onboard duty cycles
    } step_type;

    constexpr time_type kFirstTime_ms = 0; // initial time when RTE is started
    // DCC signal address for byte 0: signal 0 -> 1, signal 1 -> 2, ..., signal 7 -> 8
    const uint8 signal_byte0 = static_cast<uint8>(signal_pos + 1);
    // byte 0: accessory packet with signal address
    const uint8 byte0 = 0b10000000 | signal_byte0;
    const step_type aSteps[] =
        {
            {10, byte0, 0b11110011, 3, {2, 2, 0, 0, 2}}, 
            {20, byte0, 0b11110011, 3, {3, 3, 0, 0, 3}}, 
            {30, byte0, 0b11110011, 3, {5, 5, 0, 0, 5}},
            {40, byte0, 0b11110011, 3, {9, 9, 0, 0, 9}}, 
            {50, byte0, 0b11110011, 3, {16, 16, 0, 0, 16}}, 
            {60, byte0, 0b11110011, 3, {27, 27, 0, 0, 27}}, 
            {70, byte0, 0b11110011, 3, {48, 48, 0, 0, 48}}, 
            {80, byte0, 0b11110011, 3, {82, 82, 0, 0, 82}}, 
            {90, byte0, 0b11110011, 3, {145, 145, 0, 0, 145}}, 
            {100, byte0, 0b11110011, 3, {250, 250, 0, 0, 250}}, 
            {110, byte0, 0b11110011, 3, {255, 255, 0, 0, 255}}, 
            {120, byte0, 0b11110010, 2, {145, 145, 0, 0, 145}}, 
            {130, byte0, 0b11110010, 2, {84, 84, 0, 0, 84}}, 
            {140, byte0, 0b11110010, 2, {48, 48, 0, 0, 48}}, 
            {150, byte0, 0b11110010, 2, {28, 28, 0, 0, 28}}, 
            {160, byte0, 0b11110010, 2, {16, 16, 0, 0, 16}}, 
            {170, byte0, 0b11110010, 2, {9, 9, 0, 0, 9}}, 
            {180, byte0, 0b11110010, 2, {5, 5, 0, 0, 5}}, 
            {190, byte0, 0b11110010, 2, {3, 3, 0, 0, 3}}, 
            {200, byte0, 0b11110010, 2, {2, 2, 0, 0, 2}}, 
            {210, byte0, 0b11110010, 2, {0, 0, 0, 0, 0}}, 
            {220, byte0, 0b11110010, 2, {0, 0, 2, 2, 0}}, 
            {230, byte0, 0b11110010, 2, {0, 0, 3, 3, 0}}, 
            {240, byte0, 0b11110010, 2, {0, 0, 5, 5, 0}}, 
            {250, byte0, 0b11110010, 2, {0, 0, 9, 9, 0}}, 
            {260, byte0, 0b11110010, 2, {0, 0, 16, 16, 0}},
            {270, byte0, 0b11110010, 2, {0, 0, 27, 27, 0}}, 
            {280, byte0, 0b11110010, 2, {0, 0, 48, 48, 0}}, 
            {290, byte0, 0b11110010, 2, {0, 0, 82, 82, 0}}, 
            {300, byte0, 0b11110010, 2, {0, 0, 145, 145, 0}}, 
            {310, byte0, 0b11110010, 2, {0, 0, 250, 250, 0}}, 
            {320, byte0, 0b11110010, 2, {0, 0, 255, 255, 0}}};
            
    classifier_array_type classifiers;
    struct signal::input_cmd in;
    size_t nStep;

    // Initialize
    hal::stubs::millis = kFirstTime_ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    hal::init_gpio();

    // Here we go
    rte::start();

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // And now activate signal signal_pos
    rte::set_cv(cal::cv::kSignalIDBase + signal_pos, kBuiltInSignalIDAusfahrsignal);
    // ... with first output pin first_output_pin
    tmp = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_pos, tmp);
    // ... with DCC input
    tmp = cal::constants::make_signal_input(cal::constants::kDcc, 0);
    rte::set_cv(cal::cv::kSignalInputBase + signal_pos, tmp);

    in.type = cal::constants::kDcc;
    in.idx = signal_pos;

    for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
    {
        // Simulate DCC packet received by ISR
        {
            dcc::bit_stream_type &bit_stream = dcc::bit_streams[nStep % 2]; // Use alternating bit streams
            add_preamble_to_stream(bit_stream);
            add_byte_to_stream(bit_stream, aSteps[nStep].byte1, 0);
            add_byte_to_stream(bit_stream, aSteps[nStep].byte2, 0);
            add_byte_to_stream(bit_stream, aSteps[nStep].byte1 ^ aSteps[nStep].byte2, 1);
        }
        hal::stubs::millis = aSteps[nStep].ms;
        hal::stubs::micros = 1000U * hal::stubs::millis;
        rte::exec();
        printRte();
        uint8 cmd = rte::ifc_rte_get_cmd::call(in);
        log << std::setw(3) << (int)cmd << " - ";
        EXPECT_EQ(cmd, aSteps[nStep].cmd);
        uint8 target_pin = cal::constants::extract_signal_first_output_pin(rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_pos));
        for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
        {
            util::intensity8_255 pwm_rte;
            util::intensity8_255 pwm_hal;
            rte::ifc_onboard_target_duty_cycles::readElement(target_pin, pwm_rte);
            pwm_hal = hal::stubs::analogWrite[target_pin++];
            log << std::setw(3) << (int)pwm_rte << ", ";
            EXPECT_EQ((uint8)pwm_rte, aSteps[nStep].au8Curs[i]);
            EXPECT_EQ((uint8)pwm_hal, aSteps[nStep].au8Curs[i]);
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
void do_signal_dcc_test_aspects_0_1_UserDefined(
    const int signal_pos,
    uint8 first_output_pin,
    uint8 user_defined_signal_id,
    Logger &log)
{
    using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
    using size_type = signal_target_array::size_type;
    using time_type = classifier_array_type::classifier_type::time_type;

    uint8 tmp;

    typedef struct
    {
        time_type ms; // [ms] current time
        uint8 byte1;
        uint8 byte2;
        uint8 cmd;                   // expected value: command on RTE
        signal_target_array au8Curs; // expected value: target onboard duty cycles
    } step_type;

    constexpr time_type kFirstTime_ms = 0; // initial time when RTE is started
    // DCC signal address for byte 0: signal 0 -> 1, signal 1 -> 2, ..., signal 7 -> 8
    const uint8 signal_byte0 = static_cast<uint8>(signal_pos + 1);
    // byte 0: accessory packet with signal address
    const uint8 byte0 = 0b10000000 | signal_byte0;
    const step_type aSteps[] =
        {
            { 10, byte0, 0b11110000, 0, {  1,   0,   0,   0,   0,   0,   0,   0}}, 
            { 20, byte0, 0b11110000, 0, {  2,   0,   0,   0,   0,   0,   0,   0}}, 
            { 30, byte0, 0b11110000, 0, {  2,   0,   0,   0,   0,   0,   0,   0}},
            { 40, byte0, 0b11110000, 0, {  3,   0,   0,   0,   0,   0,   0,   0}}, 
            { 50, byte0, 0b11110000, 0, {  4,   0,   0,   0,   0,   0,   0,   0}}, 
            { 60, byte0, 0b11110000, 0, {  5,   0,   0,   0,   0,   0,   0,   0}}, 
            { 70, byte0, 0b11110000, 0, {  7,   0,   0,   0,   0,   0,   0,   0}}, 
            { 80, byte0, 0b11110000, 0, {  9,   0,   0,   0,   0,   0,   0,   0}}, 
            { 90, byte0, 0b11110000, 0, { 12,   0,   0,   0,   0,   0,   0,   0}}, 
            {100, byte0, 0b11110000, 0, { 15,   0,   0,   0,   0,   0,   0,   0}}, 
            {110, byte0, 0b11110000, 0, { 21,   0,   0,   0,   0,   0,   0,   0}}, 
            {120, byte0, 0b11110000, 0, { 27,   0,   0,   0,   0,   0,   0,   0}}, 
            {130, byte0, 0b11110000, 0, { 35,   0,   0,   0,   0,   0,   0,   0}}, 
            {140, byte0, 0b11110000, 0, { 47,   0,   0,   0,   0,   0,   0,   0}}, 
            {150, byte0, 0b11110000, 0, { 62,   0,   0,   0,   0,   0,   0,   0}}, 
            {160, byte0, 0b11110000, 0, { 81,   0,   0,   0,   0,   0,   0,   0}}, 
            {170, byte0, 0b11110000, 0, {107,   0,   0,   0,   0,   0,   0,   0}}, 
            {180, byte0, 0b11110000, 0, {142,   0,   0,   0,   0,   0,   0,   0}}, 
            {190, byte0, 0b11110000, 0, {188,   0,   0,   0,   0,   0,   0,   0}}, 
            {200, byte0, 0b11110000, 0, {244,   0,   0,   0,   0,   0,   0,   0}}, 
            {210, byte0, 0b11110000, 0, {255,   0,   0,   0,   0,   0,   0,   0}}, 
            {220, byte0, 0b11110001, 1, {192,   0,   0,   0,   0,   0,   0,   0}}, 
            {230, byte0, 0b11110001, 1, {145,   0,   0,   0,   0,   0,   0,   0}}, 
            {240, byte0, 0b11110001, 1, {109,   0,   0,   0,   0,   0,   0,   0}}, 
            {250, byte0, 0b11110001, 1, { 84,   0,   0,   0,   0,   0,   0,   0}}, 
            {260, byte0, 0b11110001, 1, { 63,   0,   0,   0,   0,   0,   0,   0}},
            {270, byte0, 0b11110001, 1, { 48,   0,   0,   0,   0,   0,   0,   0}}, 
            {280, byte0, 0b11110001, 1, { 37,   0,   0,   0,   0,   0,   0,   0}}, 
            {290, byte0, 0b11110001, 1, { 28,   0,   0,   0,   0,   0,   0,   0}}, 
            {300, byte0, 0b11110001, 1, { 21,   0,   0,   0,   0,   0,   0,   0}}, 
            {310, byte0, 0b11110001, 1, { 16,   0,   0,   0,   0,   0,   0,   0}}, 
            {320, byte0, 0b11110001, 1, { 12,   0,   0,   0,   0,   0,   0,   0}},
            {330, byte0, 0b11110001, 1, {  9,   0,   0,   0,   0,   0,   0,   0}},
            {340, byte0, 0b11110001, 1, {  7,   0,   0,   0,   0,   0,   0,   0}},
            {350, byte0, 0b11110001, 1, {  5,   0,   0,   0,   0,   0,   0,   0}},
            {360, byte0, 0b11110001, 1, {  4,   0,   0,   0,   0,   0,   0,   0}},
            {370, byte0, 0b11110001, 1, {  3,   0,   0,   0,   0,   0,   0,   0}},
            {380, byte0, 0b11110001, 1, {  2,   0,   0,   0,   0,   0,   0,   0}},
            {390, byte0, 0b11110001, 1, {  2,   0,   0,   0,   0,   0,   0,   0}},
            {400, byte0, 0b11110001, 1, {  1,   0,   0,   0,   0,   0,   0,   0}},
            {410, byte0, 0b11110001, 1, {  1,   0,   0,   0,   0,   0,   0,   0}},
            {420, byte0, 0b11110001, 1, {  0,   1,   0,   0,   0,   0,   0,   0}},
            {430, byte0, 0b11110001, 1, {  0,   2,   0,   0,   0,   0,   0,   0}},
            {440, byte0, 0b11110001, 1, {  0,   2,   0,   0,   0,   0,   0,   0}},
            {450, byte0, 0b11110001, 1, {  0,   3,   0,   0,   0,   0,   0,   0}},
            {460, byte0, 0b11110001, 1, {  0,   4,   0,   0,   0,   0,   0,   0}},
            {470, byte0, 0b11110001, 1, {  0,   5,   0,   0,   0,   0,   0,   0}},
            {480, byte0, 0b11110001, 1, {  0,   7,   0,   0,   0,   0,   0,   0}},
            {490, byte0, 0b11110001, 1, {  0,   9,   0,   0,   0,   0,   0,   0}},
            {500, byte0, 0b11110001, 1, {  0,  12,   0,   0,   0,   0,   0,   0}},
            {510, byte0, 0b11110001, 1, {  0,  15,   0,   0,   0,   0,   0,   0}},
            {520, byte0, 0b11110001, 1, {  0,  21,   0,   0,   0,   0,   0,   0}},
            {530, byte0, 0b11110001, 1, {  0,  27,   0,   0,   0,   0,   0,   0}},
            {540, byte0, 0b11110001, 1, {  0,  35,   0,   0,   0,   0,   0,   0}},
            {550, byte0, 0b11110001, 1, {  0,  47,   0,   0,   0,   0,   0,   0}},
            {560, byte0, 0b11110001, 1, {  0,  62,   0,   0,   0,   0,   0,   0}},
            {570, byte0, 0b11110001, 1, {  0,  81,   0,   0,   0,   0,   0,   0}},
            {580, byte0, 0b11110001, 1, {  0, 107,   0,   0,   0,   0,   0,   0}},
            {590, byte0, 0b11110001, 1, {  0, 142,   0,   0,   0,   0,   0,   0}},
            {600, byte0, 0b11110001, 1, {  0, 188,   0,   0,   0,   0,   0,   0}},
            {610, byte0, 0b11110001, 1, {  0, 244,   0,   0,   0,   0,   0,   0}},
            {620, byte0, 0b11110001, 1, {  0, 255,   0,   0,   0,   0,   0,   0}},
        };
    classifier_array_type classifiers;
    struct signal::input_cmd in;
    size_t nStep;

    // Initialize
    hal::stubs::millis = kFirstTime_ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    hal::init_gpio();

    // Here we go
    rte::start();

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // Configure User Defined Signal with user_defined_signal_id
    rte::set_cv(cal::eeprom::kUserDefinedSignalBase + user_defined_signal_id    , 2); // 2 LEDs
    rte::set_cv(cal::eeprom::kUserDefinedSignalBase + user_defined_signal_id + 1, 0b00000010); // aspect 0
    rte::set_cv(cal::eeprom::kUserDefinedSignalBase + user_defined_signal_id + 3, 0b00000001); // aspect 1
    rte::set_cv(cal::eeprom::kUserDefinedSignalBase + user_defined_signal_id + 5, 0b00000011); // aspect 2
    rte::set_cv(cal::eeprom::kUserDefinedSignalBase + user_defined_signal_id + 17, 20); // change over time [10 ms]
    rte::set_cv(cal::eeprom::kUserDefinedSignalBase + user_defined_signal_id + 18, 20); // change over time blink [10 ms]
    // And now activate signal signal_pos
    rte::set_cv(cal::cv::kSignalIDBase + signal_pos, cal::constants::kFirstUserDefinedSignalID + user_defined_signal_id);
    // ... with first output pin first_output_pin
    tmp = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_pos, tmp);
    // ... with DCC input
    tmp = cal::constants::make_signal_input(cal::constants::kDcc, 0);
    rte::set_cv(cal::cv::kSignalInputBase + signal_pos, tmp);

    in.type = cal::constants::kDcc;
    in.idx = signal_pos;

    for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
    {
        // Simulate DCC packet received by ISR
        {
            dcc::bit_stream_type &bit_stream = dcc::bit_streams[nStep % 2]; // Use alternating bit streams
            add_preamble_to_stream(bit_stream);
            add_byte_to_stream(bit_stream, aSteps[nStep].byte1, 0);
            add_byte_to_stream(bit_stream, aSteps[nStep].byte2, 0);
            add_byte_to_stream(bit_stream, aSteps[nStep].byte1 ^ aSteps[nStep].byte2, 1);
        }
        hal::stubs::millis = aSteps[nStep].ms;
        hal::stubs::micros = 1000U * hal::stubs::millis;
        rte::exec();
        printRte();
        uint8 cmd = rte::ifc_rte_get_cmd::call(in);
        log << std::setw(3) << (int)cmd << " - ";
        EXPECT_EQ(cmd, aSteps[nStep].cmd);
        uint8 target_pin = cal::constants::extract_signal_first_output_pin(rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_pos));
        for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
        {
            util::intensity8_255 pwm_rte;
            util::intensity8_255 pwm_hal;
            rte::ifc_onboard_target_duty_cycles::readElement(target_pin, pwm_rte);
            pwm_hal = hal::stubs::analogWrite[target_pin++];
            log << std::setw(3) << (int)pwm_rte << ", ";
            EXPECT_EQ((uint8)pwm_rte, aSteps[nStep].au8Curs[i]);
            EXPECT_EQ((uint8)pwm_hal, aSteps[nStep].au8Curs[i]);
        }
        log << std::endl;
    }
}

/**
 * @test Signal0_DCC_Aspects_0_1_UserDefinedSignal0
 * @brief Tests whether signal 7 is correctly triggered by DCC input values
 *        and whether the corresponding PWM outputs are set correctly.
 */
TEST(Ut_Signal, Signal0_DCC_Aspects_0_1_UserDefinedSignal0)
{
    Logger log;
    constexpr uint8 kUserDefinedSignalID = 0; // User Defined Signal 0
    constexpr int kSignalPos = 0; // Want to configure signal 0 as User Defined Signal kUserDefinedSignalID
    constexpr uint8 kFirstOutputPin = 13;

    log.start("Signal0_DCC_Aspects_0_1_UserDefinedSignal0.txt");
    do_signal_dcc_test_aspects_0_1_UserDefined(
        kSignalPos,
        kFirstOutputPin,
        kUserDefinedSignalID, /*cal::constants::kFirstUserDefinedSignalID is added internally */
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
void do_signal_test_red_green_builtin_3(
    const int signal_pos,
    uint8 first_output_pin,
    uint8 input_pin,
    uint8 classifier_type,
    sint8 step_size,
    Logger &log)
{
    using signal_target_array = util::array<uint8, cfg::kNrSignalTargets>;
    using size_type = signal_target_array::size_type;
    using time_type = classifier_array_type::classifier_type::time_type;

    uint8 tmp;

    typedef struct
    {
        time_type ms;                // [ms] current time
        int nPin;                    // input pin for AD value
        int nAdc;                    // current AD value for pin
        uint8 cmd;                   // expected value: command on RTE
        signal_target_array au8Curs; // expected value: target onboard duty cycles
    } step_type;

    const step_type aSteps[] =
        {
            {0, input_pin, 0, signal::kInvalidCmd, {0, 0, 0, 0, 0}}, {10, input_pin, 0, signal::kInvalidCmd, {2, 2, 0, 0, 0}}, {20, input_pin, 0, signal::kInvalidCmd, {3, 3, 0, 0, 0}}, {30, input_pin, 0, signal::kInvalidCmd, {5, 5, 0, 0, 0}}, {40, input_pin, 0, signal::kInvalidCmd, {9, 9, 0, 0, 0}}, {50, input_pin, 0, signal::kInvalidCmd, {16, 16, 0, 0, 0}}, {60, input_pin, 0, signal::kInvalidCmd, {27, 27, 0, 0, 0}}, {70, input_pin, 0, signal::kInvalidCmd, {48, 48, 0, 0, 0}}, {80, input_pin, 0, signal::kInvalidCmd, {82, 82, 0, 0, 0}}, {90, input_pin, 0, signal::kInvalidCmd, {145, 145, 0, 0, 0}}, {100, input_pin, 0, signal::kInvalidCmd, {250, 250, 0, 0, 0}}, {510, input_pin, cal::kGreenLo, signal::kInvalidCmd, {255, 255, 0, 0, 0}}, {560, input_pin, cal::kGreenLo, 1, {145, 145, 0, 0, 0}}, {570, input_pin, cal::kGreenLo, 1, {84, 84, 0, 0, 0}}, {580, input_pin, cal::kGreenLo, 1, {48, 48, 0, 0, 0}}, {590, input_pin, cal::kGreenLo, 1, {28, 28, 0, 0, 0}}, {600, input_pin, cal::kGreenLo, 1, {16, 16, 0, 0, 0}}, {610, input_pin, cal::kGreenLo, 1, {9, 9, 0, 0, 0}}, {620, input_pin, cal::kGreenLo, 1, {5, 5, 0, 0, 0}}, {630, input_pin, cal::kGreenLo, 1, {3, 3, 0, 0, 0}}, {640, input_pin, cal::kGreenLo, 1, {2, 2, 0, 0, 0}}, {650, input_pin, cal::kGreenLo, 1, {0, 0, 0, 0, 0}}, {660, input_pin, cal::kGreenLo, 1, {0, 0, 2, 0, 0}}, {670, input_pin, cal::kGreenLo, 1, {0, 0, 3, 0, 0}}, {680, input_pin, 0, signal::kInvalidCmd, {0, 0, 5, 0, 0}}, {690, input_pin, 0, signal::kInvalidCmd, {0, 0, 9, 0, 0}}, {700, input_pin, 0, signal::kInvalidCmd, {0, 0, 16, 0, 0}}, {710, input_pin, 0, signal::kInvalidCmd, {0, 0, 27, 0, 0}}, {720, input_pin, 0, signal::kInvalidCmd, {0, 0, 48, 0, 0}}, {730, input_pin, 0, signal::kInvalidCmd, {0, 0, 82, 0, 0}}, {740, input_pin, 0, signal::kInvalidCmd, {0, 0, 145, 0, 0}}, {750, input_pin, 0, signal::kInvalidCmd, {0, 0, 250, 0, 0}}, {760, input_pin, cal::kRedLo, signal::kInvalidCmd, {0, 0, 255, 0, 0}}, {810, input_pin, cal::kRedLo, 0, {0, 0, 145, 0, 0}}, {820, input_pin, 0, signal::kInvalidCmd, {0, 0, 84, 0, 0}}, {830, input_pin, 0, signal::kInvalidCmd, {0, 0, 48, 0, 0}}, {840, input_pin, 0, signal::kInvalidCmd, {0, 0, 28, 0, 0}}, {850, input_pin, 0, signal::kInvalidCmd, {0, 0, 16, 0, 0}}, {860, input_pin, 0, signal::kInvalidCmd, {0, 0, 9, 0, 0}}, {870, input_pin, 0, signal::kInvalidCmd, {0, 0, 5, 0, 0}}, {880, input_pin, 0, signal::kInvalidCmd, {0, 0, 3, 0, 0}}, {890, input_pin, 0, signal::kInvalidCmd, {0, 0, 2, 0, 0}}, {900, input_pin, 0, signal::kInvalidCmd, {0, 0, 0, 0, 0}}, {910, input_pin, 0, signal::kInvalidCmd, {2, 2, 0, 0, 0}}, {920, input_pin, 0, signal::kInvalidCmd, {3, 3, 0, 0, 0}}, {930, input_pin, 0, signal::kInvalidCmd, {5, 5, 0, 0, 0}}, {940, input_pin, 0, signal::kInvalidCmd, {9, 9, 0, 0, 0}}, {950, input_pin, 0, signal::kInvalidCmd, {16, 16, 0, 0, 0}}, {960, input_pin, 0, signal::kInvalidCmd, {27, 27, 0, 0, 0}}, {970, input_pin, 0, signal::kInvalidCmd, {48, 48, 0, 0, 0}}, {980, input_pin, 0, signal::kInvalidCmd, {82, 82, 0, 0, 0}}, {990, input_pin, 0, signal::kInvalidCmd, {145, 145, 0, 0, 0}}, {1000, input_pin, 0, signal::kInvalidCmd, {250, 250, 0, 0, 0}}};

    classifier_array_type classifiers;
    struct signal::input_cmd in;
    size_t nStep;

    // Initialize
    hal::stubs::analogRead[aSteps[0].nPin] = aSteps[0].nAdc;
    hal::stubs::millis = aSteps[0].ms;
    hal::stubs::micros = 1000U * hal::stubs::millis;
    hal::init_gpio();

    // Here we go
    rte::start();

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    // And now activate signal kSignalPos
    rte::set_cv(cal::cv::kSignalIDBase + signal_pos, kBuiltInSignalIDEinfahrsignal);
    // ... with first output pin first_output_pin
    tmp = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output_pin);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + signal_pos, tmp);
    // ... with ADC input pin input_pin
    tmp = cal::constants::make_signal_input(cal::constants::kAdc, input_pin);
    rte::set_cv(cal::cv::kSignalInputBase + signal_pos, tmp);
    // ... with classifier type classifier_type
    tmp = classifier_type;
    rte::set_cv(cal::cv::kSignalInputClassifierTypeBase + signal_pos, tmp);
    // ... with inverse output pin order and/or step size
    tmp = 0U;
    if (step_size < 0)
    {
        tmp = 0b00000001U; // set inverse order bit
    }
    // ... with step size 2
    if ((step_size == -2) || (step_size == 2))
    {
        tmp |= 0b00000010U; // set step size to 2
    } 
    rte::set_cv(cal::cv::kSignalOutputConfigBase + signal_pos, tmp);
    in.type = cal::constants::kAdc;
    in.idx = signal_pos;

    for (nStep = 0; nStep < sizeof(aSteps) / sizeof(step_type); nStep++)
    {
        hal::stubs::analogRead[aSteps[nStep].nPin] = aSteps[nStep].nAdc;
        hal::stubs::millis = aSteps[nStep].ms;
        hal::stubs::micros = 1000U * hal::stubs::millis;
        rte::exec();
        printRte();
        uint8 cmd = rte::ifc_rte_get_cmd::call(in);
        log << std::setw(3) << (int)cmd << " ";
        EXPECT_EQ(cmd, aSteps[nStep].cmd);
        uint8 target_pin = cal::constants::extract_signal_first_output_pin(rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_pos));
        for (size_type i = 0U; i < aSteps[nStep].au8Curs.size(); i++)
        {
            // Get pins for current signal target
            util::intensity8_255 pwm_rte;
            util::intensity8_255 pwm_hal;
            // Read PWM for that pin
            rte::ifc_onboard_target_duty_cycles::readElement(target_pin, pwm_rte);
            pwm_hal = hal::stubs::analogWrite[target_pin];
            target_pin = static_cast<uint8>(target_pin + step_size);
            log << std::setw(3) << (int)pwm_rte << ", ";
            //EXPECT_EQ((uint8)pwm_rte, aSteps[nStep].au8Curs[i]);
            //EXPECT_EQ((uint8)pwm_hal, aSteps[nStep].au8Curs[i]);
        }
        log << std::endl;
    }
}

/**
 * @test Signal0_ADC_Green_Red_StepSize_1
 * @brief Tests whether signal 0 is correctly triggered by ADC input values
 *        and whether the corresponding PWM outputs are set correctly.
 *        Uses step size 1 for output pins.
 */
TEST(Ut_Signal, Signal2_ADC_Green_Red_StepSize_2_BuiltIn_3)
{
    Logger log;
    constexpr int kSignalPos = 2; // Need to configure signal 2 as Einfahrsignal
    constexpr uint8 kFirstOutputPin = 22;
    constexpr uint8 kInputPin = 54;
    constexpr uint8 kClassifierType = 0;
    constexpr sint8 kStepSize = 2;

    log.start("Signal2_ADC_Green_Red_StepSize_2_BuiltIn_3.txt");

    do_signal_test_red_green_builtin_3(
        kSignalPos,
        kFirstOutputPin,
        kInputPin,
        kClassifierType,
        kStepSize,
        log);

    log.stop();
}

/**
 * @brief Tests getting and setting signal IDs in calibration variables
 * 
 * This test verifies that signal IDs can be correctly retrieved and modified
 * in the calibration variables stored in EEPROM. It checks that the default 
 * value is 'not used', and that setting a built-in signal ID works as expected.
 */
TEST(Ut_Signal, Rte_get_signal_id)
{
    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();

    for (int signal_pos = 0; signal_pos < cfg::kNrSignals; signal_pos++)
    {
        const uint16 cv_id = cal::cv::kSignalIDBase + signal_pos;
        uint8 signal_id;
        signal_id = rte::get_cv(cv_id);
        EXPECT_EQ(signal_id, cal::constants::kSignalNotUsed);
        
        rte::set_cv(cv_id, cal::constants::kFirstBuiltInSignalID);
        signal_id = rte::get_cv(cv_id);
        EXPECT_EQ(signal_id, cal::constants::kFirstBuiltInSignalID);
        
        rte::set_cv(cv_id, cal::constants::kSignalNotUsed);
        signal_id = rte::get_cv(cv_id);
        EXPECT_EQ(signal_id, cal::constants::kSignalNotUsed);
    }
}

/**
 * @brief Tests whether signal IDs are correctly identified as built-in signals
 * 
 * This test verifies that the function rte::sig::is_built_in correctly identifies
 * built-in signal IDs. It checks all built-in signal IDs and ensures that IDs outside this 
 * range are not identified as built-in.
 */
TEST(Ut_Signal, Rte_sig_is_built_in)
{
    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    uint8 signal_id;
    for (signal_id = cal::constants::kFirstBuiltInSignalID; 
         signal_id < cal::constants::kFirstBuiltInSignalID + cfg::kNrBuiltInSignals; 
         signal_id++)
    {
        EXPECT_EQ(rte::sig::is_built_in(signal_id), true);
    }
    // one past the last built-in signal ID
    EXPECT_EQ(rte::sig::is_built_in(signal_id), false);
    // one before the first built-in signal ID
    EXPECT_EQ(rte::sig::is_built_in(cal::constants::kFirstBuiltInSignalID - 1U), false);
}

/**
 * @brief Tests whether signal IDs are correctly identified as user-defined signals
 * 
 * This test verifies that the function rte::sig::is_user_defined correctly identifies
 * user-defined signal IDs. It checks all user-defined signal IDs and ensures that IDs outside this 
 * range are not identified as user-defined.
 */
TEST(Ut_Signal, Rte_sig_is_user_defined)
{
    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();
    uint8 signal_id;
    // Test all user-defined signal IDs
    for (signal_id = cal::constants::kFirstUserDefinedSignalID;
         signal_id < cal::constants::kFirstUserDefinedSignalID + cfg::kNrUserDefinedSignals;
         signal_id++)
    {
        EXPECT_EQ(rte::sig::is_user_defined(signal_id), true);
    }
    // one past the last user-defined signal ID
    EXPECT_EQ(rte::sig::is_user_defined(signal_id), false);
    // one before the first user-defined signal ID
    EXPECT_EQ(rte::sig::is_user_defined(cal::constants::kFirstUserDefinedSignalID - 1U), false);
}

/**
 * @brief Tests getting number of outputs for built-in and user-defined signals
 * 
 * This test verifies that the function rte::sig::get_number_of_outputs correctly retrieves
 * the number of outputs for both built-in and user-defined signals. It checks that the
 * number of outputs matches the expected values for built-in signals and that user-defined
 * signals return default values of zero when not configured. 
 * It also tests setting and retrieving custom number of outputs for user-defined signals.
 */
TEST(Ut_Signal, Rte_sig_get_number_of_outputs)
{
    const uint8 built_in_signal_outputs[cal::cv::kSignalLength * cfg::kNrBuiltInSignals] = CAL_BUILT_IN_SIGNAL_OUTPUTS;
    uint8 signal_id;
    for (signal_id = cal::constants::kFirstBuiltInSignalID; 
         signal_id < cal::constants::kFirstBuiltInSignalID + cfg::kNrBuiltInSignals; 
         signal_id++)
    {
        uint16 pos = signal_id - cal::constants::kFirstBuiltInSignalID;
        uint8 num_outputs = rte::sig::get_number_of_outputs(signal_id);
        EXPECT_EQ(num_outputs, built_in_signal_outputs[pos * cal::cv::kSignalLength + 0]);
    }

    for (signal_id = cal::constants::kFirstUserDefinedSignalID; 
         signal_id < cal::constants::kFirstUserDefinedSignalID + cfg::kNrUserDefinedSignals; 
         signal_id++)
    {
        const uint16 pos = signal_id - cal::constants::kFirstUserDefinedSignalID;
        // By default, number of targets should be 0 (startup/default)
        uint8 num_outputs = rte::sig::get_number_of_outputs(signal_id);
        EXPECT_EQ(num_outputs, static_cast<uint8>(0));
        // Set number of outputs to 2
        rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 0, 2); // 2 LEDs
        num_outputs = rte::sig::get_number_of_outputs(signal_id);
        EXPECT_EQ(num_outputs, static_cast<uint8>(2));
        // Restore default
        rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 0, 0); // 0 LEDs
    }
}

/**
 * @brief Tests getting signal aspects for built-in and user-defined signals
 * 
 * This test verifies that the function rte::sig::get_signal_aspect correctly retrieves
 * the signal aspects for both built-in and user-defined signals. It checks that the
 * aspects match the expected default values for built-in signals and that user-defined
 * signals return default values of zero when not configured. 
 * It also tests setting and retrieving custom aspects for user-defined signals.
 */
TEST(Ut_Signal, Rte_sig_get_signal_aspect)
{
    const uint8 built_in_signal_outputs[cal::cv::kSignalLength * cfg::kNrBuiltInSignals] = CAL_BUILT_IN_SIGNAL_OUTPUTS;
    struct signal::signal_aspect aspect;
    
    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();

    // Test for all signals
    uint8 signal_id;
    for (signal_id = cal::constants::kFirstBuiltInSignalID; 
         signal_id < cal::constants::kFirstBuiltInSignalID + cfg::kNrBuiltInSignals; 
         signal_id++)
    {
        uint8 cmd;
        // Test all aspects for built-in signals
        for (cmd = 0; cmd < cfg::kNrSignalAspects; cmd++)
        {
            uint16 pos = signal_id - cal::constants::kFirstBuiltInSignalID;
            rte::sig::get_signal_aspect(signal_id, cmd, aspect);
            EXPECT_EQ(aspect.num_targets                , built_in_signal_outputs[pos * cal::cv::kSignalLength + 0]);
            EXPECT_EQ(aspect.aspect                     , built_in_signal_outputs[pos * cal::cv::kSignalLength + 1 + cmd * 2]);
            EXPECT_EQ(aspect.blink                      , built_in_signal_outputs[pos * cal::cv::kSignalLength + 1 + cmd * 2 + 1]);
            EXPECT_EQ(aspect.change_over_time_10ms      , built_in_signal_outputs[pos * cal::cv::kSignalLength + 17]);
            EXPECT_EQ(aspect.change_over_time_blink_10ms, built_in_signal_outputs[pos * cal::cv::kSignalLength + 18]);
        }
    }

    for (signal_id = cal::constants::kFirstUserDefinedSignalID; 
         signal_id < cal::constants::kFirstUserDefinedSignalID + cfg::kNrUserDefinedSignals; 
         signal_id++)
    {
        uint8 cmd;
        const uint16 pos = signal_id - cal::constants::kFirstUserDefinedSignalID;
        // Test all aspects for user-defined signals
        for (cmd = 0; cmd < cfg::kNrSignalAspects; cmd++)
        {
            // By default, aspect should be 0 (startup/default)
            rte::sig::get_signal_aspect(signal_id, cmd, aspect);
            EXPECT_EQ(aspect.num_targets                , static_cast<uint8>(0));
            EXPECT_EQ(aspect.aspect                     , static_cast<uint8>(0));
            EXPECT_EQ(aspect.blink                      , static_cast<uint8>(0));
            EXPECT_EQ(aspect.change_over_time_10ms      , static_cast<uint8>(0));
            EXPECT_EQ(aspect.change_over_time_blink_10ms, static_cast<uint8>(0));
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 0, 2); // 2 LEDs
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 1 + cmd * 2 + 0, 0b00000010); // aspect for cmd
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 1 + cmd * 2 + 1, 0b00000100); // blink for cmd
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 17, 20); // change over time [10 ms]
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 18, 20); // change over time blink [10 ms]
            // Just set aspect for cmd, others remain 0
            for (uint8 cmd2 = 0; cmd2 < cfg::kNrSignalAspects; cmd2++)
            {
                rte::sig::get_signal_aspect(signal_id, cmd2, aspect);
                EXPECT_EQ(aspect.num_targets                , static_cast<uint8>(2));
                EXPECT_EQ(aspect.aspect                     , (cmd == cmd2) ? static_cast<uint8>(0b00000010) : static_cast<uint8>(0b00000000));
                EXPECT_EQ(aspect.blink                      , (cmd == cmd2) ? static_cast<uint8>(0b00000100) : static_cast<uint8>(0b00000000));
                EXPECT_EQ(aspect.change_over_time_10ms      , static_cast<uint8>(20));
                EXPECT_EQ(aspect.change_over_time_blink_10ms, static_cast<uint8>(20));
            }
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 0, 0); // default to 0 LEDs
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 1 + cmd * 2 + 0, 0b00000000); // default aspect for cmd
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 1 + cmd * 2 + 1, 0b00000000); // default blink for cmd
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 17, 0); // default change over time [10 ms]
            rte::set_cv(cal::eeprom::kUserDefinedSignalBase + pos * cal::cv::kSignalLength + 18, 0); // default change over time blink [10 ms]
        }
    }
}

TEST(Ut_Signal, Rte_sig_get_input)
{
    uint8 signal_pos;
    
    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();

    for (signal_pos = 0; signal_pos < cfg::kNrSignals; signal_pos++)
    {
        const uint16 cv_id = cal::cv::kSignalInputBase + signal_pos;
        uint8 input_type;
        uint8 input_pin;
        // Test setting and getting input types and indexes
        for (input_type = 0; input_type < 4; input_type++)
        {
            for (input_pin = 0; input_pin < 8; input_pin++)
            {
                uint8 tmp = cal::constants::make_signal_input(input_type, input_pin);
                rte::set_cv(cv_id, tmp);
                struct signal::input_cal read_back = rte::sig::get_input(signal_pos);
                EXPECT_EQ(read_back.type, input_type);
                EXPECT_EQ(read_back.pin, input_pin);
            }
        }
    }
}

/**
 * @brief Tests getting the first output pin configuration for signals
 * 
 * This test verifies that the function rte::sig::get_first_output correctly retrieves
 * the first output pin configuration for all signals. It tests setting various
 * first output pin values and ensures that the retrieved values match the set values.
 */
TEST(Ut_Signal, Rte_sig_get_first_output)
{
    uint8 signal_pos;

    // Initialize EEPROM with ROM default values
    rte::ifc_cal_set_defaults();

    for (signal_pos = 0; signal_pos < cfg::kNrSignals; signal_pos++)
    {
        const uint16 cv_id = cal::cv::kSignalFirstOutputBase + signal_pos;
        uint8 first_output;
        // Test setting and getting first output pin
        for (first_output = 0; first_output < 32; first_output++)
        {
            uint8 tmp = cal::constants::make_signal_first_output(cal::constants::kOnboard, first_output);
            rte::set_cv(cv_id, tmp);
            struct signal::target read_back = rte::sig::get_first_output(signal_pos);
            EXPECT_EQ(read_back.pin, first_output);
        }
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

    RUN_TEST(CalM_get_signal_id);
    RUN_TEST(CalM_update_cv_id);
    RUN_TEST(CalM_is_output_pin_step_size_1);
    RUN_TEST(CalM_is_output_pin_step_size_2);
    RUN_TEST(CalM_is_output_pin_step_size_m1);
    RUN_TEST(CalM_is_output_pin_step_size_m2);
    RUN_TEST(Signal0_ADC_Green_Red_StepSize_1);
    RUN_TEST(Signal1_ADC_Green_Red_StepSize_1);
    RUN_TEST(Signal7_ADC_Green_Red_StepSize_1);
    RUN_TEST(Signal0_ADC_Green_Red_StepSize_2);
    RUN_TEST(Signal1_ADC_Green_Red_StepSize_2);
    RUN_TEST(Signal7_ADC_Green_Red_StepSize_2);
    RUN_TEST(Signal0_ADC_Green_Red_StepSize_m1);
    RUN_TEST(Signal1_ADC_Green_Red_StepSize_m1);
    RUN_TEST(Signal7_ADC_Green_Red_StepSize_m1);
    RUN_TEST(Signal0_ADC_All);
    RUN_TEST(Signal7_ADC_All);
    RUN_TEST(Signal0_DCC_Aspects_2_3);
    RUN_TEST(Signal7_DCC_Aspects_2_3);
    RUN_TEST(Signal0_DCC_Aspects_0_1_UserDefinedSignal0);
    RUN_TEST(Signal2_ADC_Green_Red_StepSize_2_BuiltIn_3);

    RUN_TEST(Rte_get_signal_id);
    RUN_TEST(Rte_sig_is_built_in);
    RUN_TEST(Rte_sig_is_user_defined);
    RUN_TEST(Rte_sig_get_number_of_outputs);
    RUN_TEST(Rte_sig_get_signal_aspect);
    RUN_TEST(Rte_sig_get_input);
    RUN_TEST(Rte_sig_get_first_output);

    (void)UNITY_END();

    // Return false to stop program execution (relevant on Windows)
    return false;
}
