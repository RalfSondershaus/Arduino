/**
 * @file Signal/CddLed.h
 *
 * @brief Driver for LEDs
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

#ifndef CDD_LED_H_
#define CDD_LED_H_

#include <Rte/Rte_Type.h>

namespace cdd
{
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class CddLed
  {
  public:
    using intensity8_type = rte::intensity8_t;
    using intensity16_type = rte::intensity16_t;

  protected:
    void writeOutputs();

  public:
    CddLed() = default;

    /// Runables
    void init();
    void cycle();

  };
} // namespace cdd

#endif // CDD_LED_H_
