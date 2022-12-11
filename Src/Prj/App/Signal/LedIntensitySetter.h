/**
 * @file Signal/LedIntensitySetter.h
 *
 * @brief 
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

#ifndef LEDINTENSITYSETTER_H_
#define LEDINTENSITYSETTER_H_

#include <Rte/Rte_Type.h> // for intensity8_t, dimtime8_10ms_t
#include <Cal/CalM_Type.h> // for cal::target_type

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// Interface to set intensities and dim speeds
  // -----------------------------------------------------------------------------------
  class LedIntensitySetter
  {
  public:
    using dimtime8_10ms_t = rte::dimtime8_10ms_t;
    using intensity8_t = rte::intensity8_t;
    using target_type = cal::target_type;

    virtual void setIntensitySpeed(target_type tgt, intensity8_t intensity, dimtime8_10ms_t time) = 0;
  };

} // namespace signal

#endif // LEDINTENSITYSETTER_H_