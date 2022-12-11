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
  /// Intensities
  typedef uint16 intensity16_t; ///< 16 bit, 0 = 0%, 65535 = 100%
  typedef uint8  intensity8_t;  ///< 8 bit, 0 = 0%, 255 = 100%

  /// Times
  typedef uint8  dimtime8_10ms_t;  ///< 8 bit, [10 ms] 0 = 0 ms, 255 = 2550 ms = 2.55 sec
} // namespace rte

#endif // RTE_TYPE_GEN_H_
