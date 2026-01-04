/**
 * @file Signal/Com/SerAsciiTP.cpp
 *
 * @brief 
 *
 * @copyright Copyright 2023 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Com/SerAsciiTP.h>
#include <Hal/EEPROM.h>
#include <Util/ctype.h>

namespace com
{
  /// Initialization
  void SerAsciiTP::init()
  {}

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
          if (telegram_rawdata.size() > 0)
          {
            notify();
            //hal::serial::println(telegram_rawdata.c_str());
          }
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
