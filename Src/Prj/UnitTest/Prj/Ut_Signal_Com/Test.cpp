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

//-------------------------------------------------------------------------
TEST(Ut_Signal_Com, AsciiCom_process_set_signal_Aspects_1)
{
  using AsciiCom = com::AsciiCom;
  using string_type = AsciiCom::string_type;

  AsciiCom asciiCom;
  const string_type telegram = "SET SIGNAL 0 ASPECTS 11000 00000";
  asciiCom.process(telegram);
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

  RUN_TEST(AsciiCom_process_set_signal_Aspects_1);

  UNITY_END();

  // Return false to stop program execution (relevant on Windows)
  return false;
}
