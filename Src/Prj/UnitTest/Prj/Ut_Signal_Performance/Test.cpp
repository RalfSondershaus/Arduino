/**
 * @file Ut_Signal_Performance/Test.cpp
 *
 * @brief Unit tests to measure run time of project Signal
 *
 * @copyright Copyright 2025 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <unity_adapt.h>
#include <Test/Logger.h>
#include <Cal/CalM.h>
#include <Cal/CalM_config.h>
#include <Hal/EEPROM.h>
#include <Hal/Gpio.h>
#include <Hal/Timer.h>
#include <Rte/Rte.h>
#include <Rte/Rte_Cfg_Cod.h>
#include <InputClassifier.h>
#include <Util/Array.h>
#include <LedRouter.h>
#include <Dcc/Decoder.h>

#ifdef ARDUINO
#include <Arduino.h>
#endif

#define PRINT_RTE 0

static constexpr uint8 kBuiltInSignalIDAusfahrsignal = 1;

#include <Cal/CalM_config.h>

// ---------------------------------------------------------------------------
/// Set elements of RTE arrays to 0
// ---------------------------------------------------------------------------
static void cleanRte()
{
    rte::classified_values_array classified_array;
    // rte::signal_intensity_array_type signal_intensities;
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
    rte::ifc_cal_set_defaults();
    // And now activate signal kSignalPos
    rte::set_cv(cal::cv::kSignalIDBase + kSignalPos, kBuiltInSignalIDAusfahrsignal);
    EXPECT_EQ(rte::get_cv(cal::cv::kSignalIDBase + kSignalPos), kBuiltInSignalIDAusfahrsignal);
    EXPECT_EQ(rte::sig::get_signal_id(kSignalPos), kBuiltInSignalIDAusfahrsignal);
    // ... with first output pin first_output_pin
    rte::set_cv(cal::cv::kSignalFirstOutputBase + kSignalPos, cal::constants::make_signal_first_output(cal::constants::kOnboard, kFirstOutputPin));
    EXPECT_EQ(rte::sig::get_first_output(kSignalPos).pin, kFirstOutputPin);
    EXPECT_EQ(rte::sig::get_first_output(kSignalPos).type, cal::constants::kOnboard);
    // ... with ADC input pin input_pin
    rte::set_cv(cal::cv::kSignalInputBase + kSignalPos, cal::constants::make_signal_input(cal::constants::kAdc, kInputPin));
    EXPECT_EQ(rte::get_cv(cal::cv::kSignalInputBase + kSignalPos), static_cast<uint8>(cal::constants::kAdc << 6 | (kInputPin & 0x3F)));
    EXPECT_EQ(rte::sig::get_input(kSignalPos).type, cal::constants::kAdc);
    EXPECT_EQ(rte::sig::get_input(kSignalPos).pin, kInputPin);
    // ... with classifier type classifier_type
    rte::set_cv(cal::cv::kSignalInputClassifierTypeBase + kSignalPos, kClassifierType);
    EXPECT_EQ(util::classifier_cal::get_classifier_type(kSignalPos), kClassifierType);
    // ... with inverse output pin order
    rte::set_cv(cal::cv::kSignalOutputConfigBase + kSignalPos, 1U); // set inverse order bit
    EXPECT_EQ(rte::sig::is_output_pin_order_inverse(kSignalPos), true);
    // .. with step size 2
    rte::set_cv(cal::cv::kSignalOutputConfigBase + kSignalPos, 0b00000010); // set step size to 2
    EXPECT_EQ(rte::sig::get_output_pin_step_size(kSignalPos), static_cast<uint8>(2U));

    // And now activate signal kSignalPos+1
    rte::set_cv(cal::cv::kSignalIDBase + kSignalPos + 1, kBuiltInSignalIDAusfahrsignal);
    // ... with first output pin kFirstOutputPin+8
    tmp = cal::constants::make_signal_first_output(cal::constants::kOnboard, kFirstOutputPin + 8);
    rte::set_cv(cal::cv::kSignalFirstOutputBase + kSignalPos + 1, tmp);
    // ... with ADC input pin kInputPin+1
    tmp = cal::constants::make_signal_input(cal::constants::kAdc, kInputPin + 1);
    rte::set_cv(cal::cv::kSignalInputBase + kSignalPos + 1, tmp);
    // ... with classifier type kClassifierType
    tmp = kClassifierType;
    rte::set_cv(cal::cv::kSignalInputClassifierTypeBase + kSignalPos + 1, tmp);

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
        hal::stubs::analogRead[kInputPin]++;
        hal::stubs::analogRead[kInputPin + 1]++;
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
    using intensity16_type = signal::LedRouter::intensity16_type;
    using speed16_ms_type = signal::LedRouter::speed16_ms_type;

    signal::LedRouter ledr;
    constexpr int nrRep = 1000;
    struct signal::target tgt{cal::constants::make_signal_first_output(cal::constants::kOnboard, 0)};
    const intensity16_type kTgtInt{intensity16_type::kIntensity_100};
    const speed16_ms_type kTgtSpd{1};
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
    using intensity16_type = signal::LedRouter::intensity16_type;
    using speed16_ms_type = signal::LedRouter::speed16_ms_type;
    signal::LedRouter ledr;
    constexpr int nrRep = 1000;
    struct signal::target tgt(cal::constants::make_signal_first_output(cal::constants::kOnboard, 0));
    const intensity16_type kTgtInt{intensity16_type::kIntensity_100};
    const speed16_ms_type kTgtSpd{1};
    util::intensity8_255 intensity8_255;

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
    for (tgt.pin = 0; tgt.pin < cfg::kNrOnboardTargets; tgt.pin++)
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
    signal::LedRouter ledr;
    constexpr uint32 num_rep = 200; // shall not exceed kTimeBufferSize
    uint32 t1;
    uint32 t2;
    uint32 td;
    uint32 tinc;
    size_t bit_idx = 0;
    /**                         Preamble      Byte 0     Byte 1     Byte 2     */
    const char *bit_sequence = "11111111111 0 10000001 0 11110011 0 011110010 1";

    td = 0;
    hal::stubs::micros = micros();
    for (uint32 i = 0; i < num_rep; i++)
    {
        while (bit_sequence[bit_idx] == ' ')
        {
            bit_idx++;
        }
        switch (bit_sequence[bit_idx])
        {
            case '0':
                tinc = 100;
                break;
            case '1':
                tinc = 58;
                break;
            default:
                break;
        }
        bit_idx++;
        if (bit_sequence[bit_idx] == '\0')
        {
            bit_idx = 0;
        }
        t1 = micros();
        dcc::ISR_Dcc();
        hal::stubs::micros += tinc; // next bit after tinc us
        dcc::ISR_Dcc();
        t2 = micros() - t1;
        td += t2;
        hal::stubs::micros += tinc; // next bit after tinc us
    }
    hal::serial::print("ISR_Dcc1 ");
    hal::serial::println(td / (2U*num_rep)); // two calls per half bit
    hal::serial::print("Packets ");
    hal::serial::println(dcc::decoder::get_instance().get_packet_count());
    hal::serial::print("sizeof(decoder) ");
    hal::serial::println(static_cast<uint32>(sizeof(dcc::decoder)));
    hal::serial::print("sizeof(size_t) ");
    hal::serial::println(static_cast<uint32>(sizeof(size_t)));
    hal::serial::print("sizeof(uint8_least) ");
    hal::serial::println(static_cast<uint32>(sizeof(uint8_least)));
    using my_fix_deque = util::fix_deque<uint8, 1>;
    hal::serial::print("sizeof(fix_deque) ");
    hal::serial::println(static_cast<uint32>(sizeof(my_fix_deque)));
    using my_fix_queue = util::fix_queue<uint8, 1>;
    hal::serial::print("sizeof(fix_queue) ");
    hal::serial::println(static_cast<uint32>(sizeof(my_fix_queue)));
    hal::serial::print("sizeof(decoder::packet_type) ");
    hal::serial::println(static_cast<uint32>(sizeof(dcc::decoder::packet_type)));

    EXPECT_FALSE(dcc::decoder::get_instance().is_fifo_overflow());
    // empty the queue for the next run.
    dcc::decoder::get_instance().fetch();
    while (!dcc::decoder::get_instance().empty())
    {
        dcc::decoder::get_instance().pop();
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

    (void)UNITY_END();

    // Return false to stop program execution (relevant on Windows)
    return false;
}
