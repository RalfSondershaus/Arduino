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
#include <Hal/EEPROM.h>

using AsciiCom = com::AsciiCom;
using string_type = AsciiCom::string_type;

template<> void EXPECT_EQ<string_type>(string_type actual, string_type expected) { TEST_ASSERT_EQUAL_STRING(expected.c_str(), actual.c_str()); }

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_set_get_signal_Aspects_1)
{

  AsciiCom asciiCom;
  string_type telegram = "SET_SIGNAL 0 ASPECTS 11000 00100 00110 11001 11111";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase    ], static_cast<uint8>(0b11000));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 2], static_cast<uint8>(0b00100));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 4], static_cast<uint8>(0b00110));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 6], static_cast<uint8>(0b11001));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 8], static_cast<uint8>(0b11111));
  EXPECT_EQ(response, string_type("OK"));

  telegram = "GET_SIGNAL 0 ASPECTS";
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase    ], static_cast<uint8>(0b11000));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 2], static_cast<uint8>(0b00100));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 4], static_cast<uint8>(0b00110));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 6], static_cast<uint8>(0b11001));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 8], static_cast<uint8>(0b11111));
  EXPECT_EQ(response, string_type("OK 11000 00100 00110 11001 11111 "));
  
  telegram = "SET_SIGNAL 0 ASPECTS 10000 01000 00100 00010 00001";
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase    ], static_cast<uint8>(0b10000));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 2], static_cast<uint8>(0b01000));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 4], static_cast<uint8>(0b00100));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 6], static_cast<uint8>(0b00010));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 8], static_cast<uint8>(0b00001));
  EXPECT_EQ(response, string_type("OK"));

  telegram = "GET_SIGNAL 0 ASPECTS";
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase    ], static_cast<uint8>(0b10000));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 2], static_cast<uint8>(0b01000));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 4], static_cast<uint8>(0b00100));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 6], static_cast<uint8>(0b00010));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 8], static_cast<uint8>(0b00001));
  EXPECT_EQ(response, string_type("OK 10000 01000 00100 00010 00001 "));
}

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_get_signal_Aspects_1)
{
  using AsciiCom = com::AsciiCom;
  using string_type = AsciiCom::string_type;

  AsciiCom asciiCom;
  const string_type telegram = "SET_SIGNAL 0 ASPECTS 11000 00100";
  string_type response;
  asciiCom.process(telegram, response);
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase    ], static_cast<uint8>(0b11000));
  EXPECT_EQ(hal::eeprom::stubs::elements[cal::eeprom::eSignalBase + cal::eeprom::eSignalAspectBase + 1], static_cast<uint8>(0b00000));
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

  RUN_TEST(AsciiCom_process_set_get_signal_Aspects_1);

  UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
