/**
  * @file String.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief Utility functions for string operations
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

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Algorithm.h>
#include <Util/Array.h>
#include <Util/Math.h>
#include <Util/String.h>

namespace util
{
  // ------------------------------------------------------------------------------
  /// Returns true if the character ch is a null character, and false otherwise.
  // ------------------------------------------------------------------------------
  bool isnull(char ch)
  {
    return ch == '\0';
  }

  // ------------------------------------------------------------------------------
  /// Returns 0 if the character ch is a white space character, and a non-zero value otherwise.
  /// 
  /// @param ch [in] character to classify
  /// 
  /// @return 0 if the character ch is a white space character, and a non-zero value otherwise.
  // ------------------------------------------------------------------------------
  int isspace(int ch)
  {
    using chararray = util::array<unsigned char, 6>;
    static const chararray ws =
    {
      ' ',  // space (0x20)
      '\f', // form feed (0x0c)
      '\n', // line feed (0x0a)
      '\r', // carriage return (0x0d)
      '\t', // horizontal tab (0x09)
      '\v'  // vertical tab (0x0b)
    };

    return util::find(ws.begin(), ws.end(), static_cast<unsigned char>(ch)) != ws.end();
  }
 
} // namespace util
