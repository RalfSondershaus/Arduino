/**
  * @file Algorithm.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief Algorithms
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

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Algorithm.h>

namespace util
{
  // ------------------------------------------------------------------------------
  /// Copies count bytes from the object pointed to by src to the object pointed to 
  /// by dest. Both objects are reinterpreted as arrays of unsigned char.
  /// Returns dest.
  // ------------------------------------------------------------------------------
  void* memcpy(void* dest, const void* src, size_t count)
  {
    uint8* pDest = static_cast<uint8*>(dest);
    const uint8* pSrc = static_cast<const uint8*>(src);

    while (count > 0U)
    {
      *pDest = *pSrc;
      count--;
    }

    return dest;
  }
} // namespace util
