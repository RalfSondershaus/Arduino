/**
 * @file Gen/Rte/Rte_Type_Gen.h
 *
 * @brief Defines generic types for RTE
 *
 * @copyright Copyright 2022 Ralf Sondershaus
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

#ifndef RTE_TYPE_GEN_H_
#define RTE_TYPE_GEN_H_

#include <Std_Types.h>

namespace rte
{
  using ret_type = ifc_base::ret_type;

  constexpr uint8 kIntensity8_0 = 0U;
  constexpr uint8 kIntensity8_100 = 128U;

  constexpr uint8 kIntensity8_255_0 = 0U;
  constexpr uint8 kIntensity8_255_100 = 255U;

  constexpr uint16 kIntensity16_0 = 0x0000U;
  constexpr uint16 kIntensity16_100 = 0x8000U;

  class intensity8;
  class intensity8_255;
  class intensity16;

  // 
  // -----------------------------------------------------------------------------
  /// 8 bit intensity: [0 (0%) ... 255 (100%)] 
  // -----------------------------------------------------------------------------
  class intensity8_255
  {
  public:
    typedef uint8 base_type;
    typedef intensity8_255 This;

    base_type v;

    /// Constructors
    intensity8_255() = default;
    explicit constexpr intensity8_255(base_type i) : v(i) {}    // explicit to avoid implicit type conversions of i
    intensity8_255(const intensity8_255& copy) : v(copy.v) {}

    static constexpr base_type kIntensity_0 = kIntensity8_255_0;
    static constexpr base_type kIntensity_100 = kIntensity8_255_100;

    static constexpr This intensity_0()   { return This{kIntensity_0  }; }
    static constexpr This intensity_100() { return This{kIntensity_100}; }

    /// Sets intensity.
    This& operator=(const base_type i) noexcept { v = i; return *this; }
    This& operator=(const intensity8_255& copy) noexcept { v = copy.v; return *this; }

    /// Convert to the base type (uint8)
    constexpr operator base_type() const { return v; }
  };

  // -----------------------------------------------------------------------------
  /// 8 bit intensity: [0 (0%) ... 128 (100%)] 
  // -----------------------------------------------------------------------------
  class intensity8
  {
  public:
    typedef uint8 base_type;
    typedef intensity8 This;

    static constexpr base_type kIntensity_0 = kIntensity8_0;
    static constexpr base_type kIntensity_100 = kIntensity8_100;

    static constexpr This intensity_0  () { return This{kIntensity_0  }; }
    static constexpr This intensity_100() { return This{kIntensity_100}; }

    base_type v;

    /// Constructors
    intensity8() = default;
    explicit constexpr intensity8(base_type i) : v(i) {}    // explicit to avoid implicit type conversions of i

    /// Sets intensity to intensity i.
    /// 
    /// @param i Intensity to be assigned
    /// 
    /// @return this
    This& operator=(const base_type i) noexcept { v = i; return *this; }

    /// Convert to the base type (uint8)
    constexpr operator base_type() const { return v; }
  };

  // -----------------------------------------------------------------------------
  /// 16 bit intenity: [0x0000 (0%) ... 0x8000 (100%)]
  // -----------------------------------------------------------------------------
  class intensity16
  {
  public:
    typedef uint16 base_type;
    typedef intensity16 This;

    static constexpr base_type kIntensity_0 = kIntensity16_0;
    static constexpr base_type kIntensity_100 = kIntensity16_100;

    static constexpr This intensity_0  () { return This{kIntensity_0  }; }
    static constexpr This intensity_100() { return This{kIntensity_100}; }

    base_type v;

    /// Constructors
    intensity16() = default;
    explicit constexpr intensity16(base_type i) : v(i) {}    // explicit to avoid implicit type conversions of i

    
    /// Sets intensity to intensity i.
    /// 
    /// @param i Intensity to be assigned
    /// 
    /// @return this
    This& operator=(const base_type i) noexcept { v = i; return *this; }

    /// Convert to the base type (uint16)
    constexpr operator base_type() const { return v; }
  };

  // §7.1.5/2 in the C++11 standard: "constexpr functions and constexpr constructors are implicitly inline (7.1.2).

  /// Default converter (returns default object currently).
  template<class Dest, class Source>
  inline constexpr Dest convert(const Source& src) { return Dest(); }

  /// Returns the 16 bit intensity from 8 bit intensity with 100%=255
  template<>
  inline constexpr intensity16 convert<intensity16, intensity8_255>(const intensity8_255& src) { return static_cast<intensity16>((src * kIntensity16_100) / 255U); }

  /// Returns the 16 bit intensity from 8 bit intensity with 100%=128
  template<>
  inline constexpr intensity16 convert<intensity16, intensity8>(const intensity8& src) { return static_cast<intensity16>(src * (kIntensity16_100 / kIntensity8_100)); }

  /// Returns the 8 bit intensity (100% = 128) from 16 bit intensity
  template<>
  inline constexpr intensity8 convert<intensity8, intensity16>(const intensity16& src) { return intensity8(static_cast<intensity8::base_type>(src / (kIntensity16_100 / kIntensity8_100))); }

  /// Returns the 8 bit intensity (100% = 255) from 16 bit intensity
  template<>
  inline constexpr intensity8_255 convert<intensity8_255, intensity16>(const intensity16& src) { return intensity8_255(static_cast<intensity8_255::base_type>((src * kIntensity8_255_100) / kIntensity16_100)); }

  /// 16 bit, [./ms], 0 = zero speed, 65535 = 65535 / ms
  /// If used for intensity:
  /// - 0x8000 / ms is 100% / ms (fastest, see also kSpeed16Max)
  /// - 0x4000 / ms is  50% / ms
  /// - 0x0001 / ms is  0.000030517% / ms = 0.03% / s = 1.83% / min (slowest).
  /// Dim time when switching from 0% to 100% intensity (0x0 ... 0x8000):
  /// - 0x1000: 0.125% / ms             =>      8 ms
  /// - 0x0147: 0.01% / ms              =>    100 ms
  /// - 0x0100: 0.0078125% / ms         =>    128 ms
  /// - 0x00A7:                         =>    200 ms
  /// - 0x0010: 0.00048828125% / ms     =>  2,048 ms ~ 2 sec
  /// - 0x0001: 0.000030517578125% / ms => 32,768 ms ~ 32 sec
  /// Formulas:
  /// - HEX -> Dim time (ms): T = 0x8000 / H
  /// - Dim time (ms) -> HEX: H = 0x8000 / T
  typedef uint16 speed16_ms_t;
  
  /// If used for intensity: maximal speed
  constexpr speed16_ms_t kSpeed16Max = 0x8000;

  /// Times
  typedef uint8  dimtime8_10ms_t;  ///< 8 bit, [10 ms] 0 = 0 ms, 255 = 2550 ms = 2.55 sec
} // namespace rte

#endif // RTE_TYPE_GEN_H_
