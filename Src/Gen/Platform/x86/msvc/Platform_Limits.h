/**
 * Substitude for <numerical_limit>
 * 
 * @file Platform/avr/avrgcc/Platform_Limits.h
 */

#ifndef LIMITS_H_
#define LIMITS_H_

#include "Platform_Types.h"
#include <limits>

namespace platform
{
  template<class T> class numeric_limits
  {
  public:
    /// min and max. Postfix _ is required because of old compiler #defines (for example, avr gcc)
    static constexpr T min_() noexcept { return T(); }
    static constexpr T max_() noexcept { return T(); }
  };

  template<> class numeric_limits<uint8>
  {
  public:
    static constexpr uint8 min_() noexcept { return std::numeric_limits<uint8>::min(); }
    static constexpr uint8 max_() noexcept { return std::numeric_limits<uint8>::max(); }
  };
} // namespace util

#endif // LIMITS_H_
