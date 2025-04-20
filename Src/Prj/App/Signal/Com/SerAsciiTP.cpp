/**
 * @file Signal/Com/SerAsciiTP.cpp
 *
 * @brief 
 *
 * @copyright Copyright 2023 Ralf Sondershaus
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

#include <Com/SerAsciiTP.h>
#include <Hal/EEPROM.h>
#include <Util/ctype.h>

namespace com
{
  /// Initialization
  void SerAsciiTP::init()
  {}

  // SET SIGNAL 0 ASPECTS 11000 00000 00100 00000 00110 00000 11001 00000 11111 00000
  /// Receive data from low level drivers and process them
  void SerAsciiTP::cycle()
  {
    if (driver)
    {
      bool cntrlFound = false;

      while (driver->available())
      {
        int b = driver->read();
        if (util::iscntrl(b))
        {
          cntrlFound = true;
          break;
        }
        else if (!bOverflow)
        {
          if (telegram_rawdata.size() < telegram_rawdata.max_size())
          {
            if (util::isalnum(b) || util::isspace(b) || util::ispunct(b))
            {
              telegram_rawdata += string_type::traits_type::to_char_type(b);
            }
          }
          else
          {
            bOverflow = true;
            hal::serial::print("ERR: message too long: ");
            hal::serial::println(telegram_rawdata.c_str());
            break;
          }
        }
        else
        {
          // skip characters until cntrl is found
        }
      }

      if (cntrlFound)
      {
        if (!bOverflow)
        {
          notify();
          hal::serial::println(telegram_rawdata.c_str());
        }
        else
        {
          bOverflow = false;
        }
        telegram_rawdata.clear();
      }
    }
  }

  /// Transmit a character string to the serial port and append a backslash.
  void SerAsciiTP::transmitTelegram(const string_type& telegram)
  {
    if (driver)
    {
      driver->write(telegram.c_str());
      driver->write("\r\n");
    }
  }

} // namespace com
