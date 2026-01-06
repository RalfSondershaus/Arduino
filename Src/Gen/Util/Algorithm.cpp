/**
  * @file Algorithm.cpp
  *
  * @author Ralf Sondershaus
  *
  * @brief Algorithms
  *
  * @copyright Copyright 2023 Ralf Sondershaus
  *
  * SPDX-License-Identifier: Apache-2.0
  */

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Util/Algorithm.h>

namespace util
{
  #if 0
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
  #endif
} // namespace util
