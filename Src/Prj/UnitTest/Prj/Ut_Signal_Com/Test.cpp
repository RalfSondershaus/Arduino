 /**
 * @file Test.cpp
   *
   * @author Ralf Sondershaus
   *
   * Unit Test for Prj/App/Signal/Com/...
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
   * You should have received a copy of the GNU General Public License
   * along with this program.  If not, see <https://www.gnu.org/licenses/>.
   */

#include <unity_adapt.h>
#include <Com/AsciiCom.h>
#include <Cal/CalM_config.h>
#include <Rte/Rte.h>
#include <Hal/EEPROM.h>

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
  RUN_TEST(AsciiCom_process_INIT);

  UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
