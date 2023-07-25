/**
 * Substitude for <numerical_limit>
 * 
 * @file Platform/avr/avrgcc/Platform_Limits.h
 */

#ifndef PLATFORM_LIMITS_H_
#define PLATFORM_LIMITS_H_

#include "Platform_Types.h"

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
    static constexpr uint8 min_() noexcept { return static_cast<uint8>(0); }
    static constexpr uint8 max_() noexcept { return UINT8_MAX; }
  };

  template<> class numeric_limits<uint16>
  {
  public:
    static constexpr uint8 min_() noexcept { return static_cast<uint16>(0); }
    static constexpr uint8 max_() noexcept { return UINT16_MAX; }
  };
} // namespace util

#endif // PLATFORM_LIMITS_H_
