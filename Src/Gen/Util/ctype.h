/**
  * @file ctype.h
  *
  * @author Ralf Sondershaus
  *
  * @brief Character classification functions as classified by the currently installed C locale.
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

#ifndef UTIL_CTYPE_H
#define UTIL_CTYPE_H

#include <ctype.h>

namespace util
{
  /// isalnum: 
  /// - digits (0123456789)
  /// - uppercase letters (ABCDEFGHIJKLMNOPQRSTUVWXYZ)
  /// - lowercase letters (abcdefghijklmnopqrstuvwxyz)

  using ::isalnum;
  using ::isalpha;
  using ::isspace;
  using ::iscntrl;
} // namespace util

#endif // UTIL_CTYPE_H