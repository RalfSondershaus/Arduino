/**
 * Substitude for <numerical_limit>
 * 
 * @file Platform/x86/gcc/Platform_Limits.h
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef LIMITS_H_
#define LIMITS_H_

#include "Platform_Types.h"
#include <limits>

/* If the compiler doesn't support CHAR_BIT */
#ifndef CHAR_BIT
#define CHAR_BIT     8
#endif

namespace platform
{
  template<class T> class numeric_limits
  {
  public:
    /// min and max. Postfix _ is required because of old compiler #defines (for example, avr gcc)
    static constexpr T min_() noexcept { return std::numeric_limits<T>::min(); }
    static constexpr T max_() noexcept { return std::numeric_limits<T>::max(); }
  };

  template<> class numeric_limits<uint8>
  {
  public:
    static constexpr uint8 min_() noexcept { return std::numeric_limits<uint8>::min(); }
    static constexpr uint8 max_() noexcept { return std::numeric_limits<uint8>::max(); }
    static constexpr int digits = CHAR_BIT;
  };
} // namespace util

#endif // LIMITS_H_
