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

      while (driver->available() && (telegram_rawdata.size() < telegram_rawdata.max_size()))
      {
        int b = driver->read();

        if (iscntrl(b))
        {
          cntrlFound = true;
          break;
        }
        else
        {
          telegram_rawdata += (telegram_base_type) b;
        }
      }

      if (cntrlFound)
      {
        if (!bOverflow)
        {
          notify();
        }
        else
        {
          bOverflow = false;
        }
      }
      else
      {
        // Check for messages that are longer than our buffer, and discard them
        if (telegram_rawdata.size() >= telegram_rawdata.max_size())
        {
          telegram_rawdata.clear();
          bOverflow = true;
        }
      }
    }
  }

} // namespace com
