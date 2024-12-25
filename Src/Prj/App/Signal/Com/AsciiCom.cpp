/**
 * @file Signal/Com/AsciiCom.cpp
 *
 * @brief
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
 * See <https://www.gnu.org/licenses/>.
 */

#include <Com/AsciiCom.h>

namespace com
{

  // -----------------------------------------------------------------------------------
  /// <payload>'\n'
  ///
  /// With <payload> such as:
  ///
  /// \code
  /// SET CAL SIGNAL.0.ASPECT.0.ASPECT 11000
  /// SET CAL SIGNAL.0.ASPECT.0.BLINK 00000
  /// SET CAL SIGNAL.0.ASPECT.1.ASPECT 00100
  /// SET CAL SIGNAL.0.ASPECT.2.ASPECT 00110
  /// SET CAL SIGNAL.0.ASPECT.3.ASPECT 11001
  /// SET CAL SIGNAL.0.ASPECT.4.ASPECT 11111
  /// SET CAL SIGNAL.0.ASPECT.4.BLINK 11111
  /// SET CAL SIGNAL.0.TARGET.0 onboard 13
  /// SET CAL SIGNAL.0.TARGET.1 onboard 12
  /// SET CAL SIGNAL.0.TARGET.2 onboard 11
  /// SET CAL SIGNAL.0.TARGET.3 onboard 10
  /// SET CAL SIGNAL.0.TARGET.4 onboard 9               set target port 0 to (onboard, pin9) for signal 0
  ///
  /// GET CAL SIGNAL.0.ASPECT                           returns all (5) aspects for signal 0
  /// GET CAL SIGNAL.0.TARGET                           returns all (5) target ports for signal 0
  ///
  /// CPY CAL SIGNAL.0 SIGNAL.1                         copies all aspects and target ports from signal 0 to signal 1
  /// \code
  // -----------------------------------------------------------------------------------
  void AsciiCom::update()
  {
    if (asciiTP)
    {
      const string_type& telegram = asciiTP->getTelegram();
      process(telegram);
    }
  }

  // -----------------------------------------------------------------------------------
  ///
  // -----------------------------------------------------------------------------------
  void AsciiCom::process(const string_type& telegram)
  {
    
  }

} // namespace com
