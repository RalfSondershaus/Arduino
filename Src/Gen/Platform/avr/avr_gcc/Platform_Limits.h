/**
 * Substitude for <numerical_limit>
 * 
 * @file Platform/avr/avr_gcc/Platform_Limits.h
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
    static constexpr T min_() noexcept {  }
    static constexpr T max_() noexcept {  }
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
    static constexpr uint16 min_() noexcept { return static_cast<uint16>(0); }
    static constexpr uint16 max_() noexcept { return UINT16_MAX; }
  };

  template<> class numeric_limits<uint32>
  {
  public:
    static constexpr uint32 min_() noexcept { return static_cast<uint32>(0); }
    static constexpr uint32 max_() noexcept { return UINT32_MAX; }
  };

  template<> class numeric_limits<uint64>
  {
  public:
    static constexpr uint64 min_() noexcept { return static_cast<uint64>(0); }
    static constexpr uint64 max_() noexcept { return UINT64_MAX; }
  };

  template<> class numeric_limits<sint8>
  {
  public:
    static constexpr sint8 min_() noexcept { return INT8_MIN; }
    static constexpr sint8 max_() noexcept { return INT8_MAX; }
  };

  template<> class numeric_limits<sint16>
  {
  public:
    static constexpr sint16 min_() noexcept { return INT16_MIN; }
    static constexpr sint16 max_() noexcept { return INT16_MAX; }
  };

  template<> class numeric_limits<sint32>
  {
  public:
    static constexpr sint32 min_() noexcept { return INT32_MIN; }
    static constexpr sint32 max_() noexcept { return INT32_MAX; }
  };

  template<> class numeric_limits<sint64>
  {
  public:
    static constexpr sint64 min_() noexcept { return INT64_MIN; }
    static constexpr sint64 max_() noexcept { return INT64_MAX; }
  };
} // namespace util

#endif // PLATFORM_LIMITS_H_
