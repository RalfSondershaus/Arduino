/**
 * @file Hal/Win/Serial/Hal/Serial.cpp
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

#include <Std_Types.h>
#include <Hal/Serial.h>

namespace hal
{
  namespace serial 
  {
    void begin(long baudrate)   {}
    int available()             { return 0; }
    int read()                  { return 0; }
  }
} // namespace com
