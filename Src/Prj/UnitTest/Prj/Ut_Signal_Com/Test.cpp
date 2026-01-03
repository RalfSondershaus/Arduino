 /**
   * @file Test.cpp
   *
   * @author Ralf Sondershaus
   *
   * @brief Test for Prj/App/Signal/Com/...
   *
   * @copyright Copyright 2024 Ralf Sondershaus
   *
   * SPDX-License-Identifier: Apache-2.0
   */

#include <unity_adapt.h>
#include <Com/AsciiCom.h>
#include <Cal/CalM_config.h>
#include <Rte/Rte.h>
#include <Hal/EEPROM.h>
#include <Rte/Rte_Cfg_Cod.h>

using AsciiCom = com::AsciiCom;
using string_type = AsciiCom::string_type;

template<> void EXPECT_EQ<string_type>(string_type actual, string_type expected) { TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str()); }

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_SET_CV_SignalIDs)
{
  AsciiCom asciiCom;
  string_type telegram = "SET_CV 42 1";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalIDBase + 0], static_cast<uint8>(1));
  EXPECT_EQ(response, string_type("OK SET_CV 42 1"));

  telegram = "SET_CV 43 1";
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalIDBase + 1], static_cast<uint8>(1));
  EXPECT_EQ(response, string_type("OK SET_CV 43 1"));
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_SET_SIGNAL_ONB_ADC)
{
  AsciiCom asciiCom;
  string_type telegram = "SET_SIGNAL 0 1 ONB 10 -1 ADC 54";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalIDBase + 0], static_cast<uint8>(1));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalFirstOutputBase + 0], cal::constants::make_signal_first_output(cal::constants::kOnboard, 10U));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalOutputConfigBase + 0], static_cast<uint8>(1));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalInputBase + 0], cal::constants::make_signal_input(cal::constants::kAdc, 54U));
  EXPECT_EQ(response, string_type("OK SET_SIGNAL 0 1 ONB 10 -1 ADC 54"));
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_SET_SIGNAL_ONB_DCC)
{
  AsciiCom asciiCom;
  string_type telegram = "SET_SIGNAL 0 1 ONB 10 -1 DCC 54";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalIDBase + 0], static_cast<uint8>(1));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalFirstOutputBase + 0], cal::constants::make_signal_first_output(cal::constants::kOnboard, 10U));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalOutputConfigBase + 0], static_cast<uint8>(1));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalInputBase + 0], cal::constants::make_signal_input(cal::constants::kDcc, 54U));
  EXPECT_EQ(response, string_type("OK SET_SIGNAL 0 1 ONB 10 -1 DCC 54"));
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_SET_SIGNAL_EXT_DIG)
{
  AsciiCom asciiCom;
  string_type telegram = "SET_SIGNAL 0 1 EXT 10 2 DIG 22";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalIDBase + 0], static_cast<uint8>(1));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalFirstOutputBase + 0], cal::constants::make_signal_first_output(cal::constants::kExternal, 10U));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalOutputConfigBase + 0], static_cast<uint8>(2));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalInputBase + 0], cal::constants::make_signal_input(cal::constants::kDig, 22U));
  EXPECT_EQ(response, string_type("OK SET_SIGNAL 0 1 EXT 10 2 DIG 22"));
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_ETO_SET_SIGNAL)
{
  AsciiCom asciiCom;
  uint8 cmd = 0;
  signal::signal_aspect aspect;
  constexpr uint8 aspect_builtin = 0b00011000;
  constexpr uint8 dim_time_builtin = 10;
  string_type telegram1 = "SET_SIGNAL 0 1 ONB 10 2 ADC 54";
  string_type telegram2 = "ETO_SET_SIGNAL 0 1 5";
  string_type telegram3 = "ETO_SET_SIGNAL 0 0";
  string_type response;

  // First activate signal idx 0 to built-in signal id 1 have a valid initial state
  asciiCom.process(telegram1, response);
  EXPECT_EQ(response, string_type("OK SET_SIGNAL 0 1 ONB 10 2 ADC 54"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, aspect_builtin);
  EXPECT_EQ(aspect.change_over_time_10ms, dim_time_builtin);

  // Then set ETO signal aspect for signal idx 0: aspect 1, dim time 5
  asciiCom.process(telegram2, response);
  EXPECT_EQ(response, string_type("OK ETO_SET_SIGNAL 0 1 5"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, static_cast<uint8>(1U));
  EXPECT_EQ(aspect.change_over_time_10ms, static_cast<uint8>(5U));

  // Then disable ETO signal aspect for signal idx 0
  asciiCom.process(telegram3, response);
  EXPECT_EQ(response, string_type("OK ETO_SET_SIGNAL 0 0"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, aspect_builtin);
  EXPECT_EQ(aspect.change_over_time_10ms, dim_time_builtin);
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_ETO_SET_SIGNAL_OPTIONAL_DIM_TIME)
{
  AsciiCom asciiCom;
  uint8 cmd = 0;
  signal::signal_aspect aspect;
  constexpr uint8 aspect_builtin = 0b00011000;
  constexpr uint8 dim_time_builtin = 10;
  string_type telegram1 = "SET_SIGNAL 0 1 ONB 10 2 ADC 54";
  string_type telegram2 = "ETO_SET_SIGNAL 0 1";
  string_type telegram3 = "ETO_SET_SIGNAL 0 0";
  string_type response;
  
  // First activate signal idx 0 to built-in signal id 1 have a valid initial state
  asciiCom.process(telegram1, response);
  EXPECT_EQ(response, string_type("OK SET_SIGNAL 0 1 ONB 10 2 ADC 54"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, aspect_builtin);
  EXPECT_EQ(aspect.change_over_time_10ms, dim_time_builtin);

  // Then set ETO signal aspect for signal idx 0: aspect 1, dim time 10 (default)
  asciiCom.process(telegram2, response);
  EXPECT_EQ(response, string_type("OK ETO_SET_SIGNAL 0 1"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, static_cast<uint8>(1U));
  EXPECT_EQ(aspect.change_over_time_10ms, static_cast<uint8>(10U));

  // Then disable ETO signal aspect for signal idx 0
  asciiCom.process(telegram3, response);
  EXPECT_EQ(response, string_type("OK ETO_SET_SIGNAL 0 0"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, aspect_builtin);
  EXPECT_EQ(aspect.change_over_time_10ms, dim_time_builtin);
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_ETO_SET_SIGNAL_INVALID_IDX)
{
  AsciiCom asciiCom;
  uint8 cmd = 0;
  signal::signal_aspect aspect;
  constexpr uint8 aspect_builtin = 0b00011000;
  constexpr uint8 dim_time_builtin = 10;
  string_type telegram1 = "SET_SIGNAL 0 1 ONB 10 2 ADC 54";
  string_type telegram2 = "ETO_SET_SIGNAL 100 1";
  string_type response;
  
  // First activate signal idx 0 to built-in signal id 1 have a valid initial state
  asciiCom.process(telegram1, response);
  EXPECT_EQ(response, string_type("OK SET_SIGNAL 0 1 ONB 10 2 ADC 54"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, aspect_builtin);
  EXPECT_EQ(aspect.change_over_time_10ms, dim_time_builtin);

  // Then set ETO signal aspect for signal idx 0: aspect 1, dim time 10 (default)
  asciiCom.process(telegram2, response);
  EXPECT_EQ(response, string_type("ERR: Invalid signal index ETO_SET_SIGNAL 100 1"));
  rte::sig::get_signal_aspect_for_idx(0, cmd, aspect);
  EXPECT_EQ(aspect.aspect, aspect_builtin);
  EXPECT_EQ(aspect.change_over_time_10ms, dim_time_builtin);
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_INIT)
{
  AsciiCom asciiCom;
  string_type telegram = "INIT";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kDecoderAddressLSB], cal::kAddressLSB);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kDecoderAddressMSB], cal::kAddressMSB);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kManufacturerVersionID], cal::kManufacturerVersionID);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kManufacturerID], cal::kManufacturerID);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kConfiguration], cal::kConfiguration);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kManufacturerCVStructureID], cal::kManufacturerCVStructureID);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalIDBase + 0], cal::constants::kSignalNotUsed);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::kSignalIDBase + 1], cal::constants::kSignalNotUsed);
  EXPECT_EQ(response, string_type("OK INIT"));
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

  RUN_TEST(AsciiCom_process_SET_CV_SignalIDs);
  RUN_TEST(AsciiCom_process_SET_SIGNAL_ONB_ADC);
  RUN_TEST(AsciiCom_process_SET_SIGNAL_ONB_DCC);
  RUN_TEST(AsciiCom_process_SET_SIGNAL_EXT_DIG);
  RUN_TEST(AsciiCom_process_ETO_SET_SIGNAL);
  RUN_TEST(AsciiCom_process_ETO_SET_SIGNAL_OPTIONAL_DIM_TIME);
  RUN_TEST(AsciiCom_process_ETO_SET_SIGNAL_INVALID_IDX);
  RUN_TEST(AsciiCom_process_INIT);

  UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
