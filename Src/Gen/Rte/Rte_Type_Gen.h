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

  /// Intensities
  typedef uint16 intensity16_t; ///< 16 bit, 0x0000 = 0%, 0x8000 = 100% (32768)
  typedef uint8  intensity8_t;  ///< 8 bit, 0 = 0%, 100 = 100%

  constexpr intensity8_t kIntensity8_0 = 0U;
  constexpr intensity8_t kIntensity8_100 = 100U;

  constexpr intensity16_t kIntensity16_0 = 0U;
  constexpr intensity16_t kIntensity16_100 = 0x8000U;

  /// 16 bit, [./ms], 0 = zero speed, 65535 = 65535 / ms
  /// If used for intensity:
  /// - 0x8000 / ms is 100% / ms (fastest)
  /// - 0x4000 / ms is  50% / ms
  /// - 0x0001 / ms is  0.000030517% / ms = 0.03% / s = 1.83% / min (slowest)
  typedef uint16 speed16_ms_t;

  /// Times
  typedef uint8  dimtime8_10ms_t;  ///< 8 bit, [10 ms] 0 = 0 ms, 255 = 2550 ms = 2.55 sec
} // namespace rte

#endif // RTE_TYPE_GEN_H_
