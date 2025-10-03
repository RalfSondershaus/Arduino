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

/**
 * @brief Stub function
 * 
 * @param classifier_pos 
 * @return rte::ret_type 
 */
rte::ret_type signal::InputClassifier::set_config(uint8 classifier_pos) 
{
    (void) classifier_pos;
    return rte::ret_type::OK;
}

template<> void EXPECT_EQ<string_type>(string_type actual, string_type expected) { TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str()); }

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_SET_CV_SignalIDs)
{
  AsciiCom asciiCom;
  string_type telegram = "SET_CV 42 1";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalIDBase + 0], static_cast<uint8>(1));
  EXPECT_EQ(response, string_type("OK SET_CV 42 1"));

  telegram = "SET_CV 43 1";
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalIDBase + 1], static_cast<uint8>(1));
  EXPECT_EQ(response, string_type("OK SET_CV 43 1"));
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_INIT)
{
  AsciiCom asciiCom;
  string_type telegram = "INIT";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eDecoderAddressLSB], cal::kAddressLSB);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eDecoderAddressMSB], cal::kAddressMSB);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eManufacturerVersionID], cal::kManufacturerVersionID);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eManufacturerID], cal::kManufacturerID);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eConfiguration], cal::kConfiguration);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eManufacturerCVStructureID], cal::kManufacturerCVStructureID);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalIDBase + 0], cal::kSignalNotUsed);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalIDBase + 1], cal::kSignalNotUsed);
  EXPECT_EQ(response, string_type("OK"));
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
  RUN_TEST(AsciiCom_process_INIT);

  UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
