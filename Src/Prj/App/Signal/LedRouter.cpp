/**
 * @file Signal/LedRouter.cpp
 *
 * @brief Routes classified values (ifc_classified_values) to target values.
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

#include <Rte/Rte.h>
#include <Util/Algorithm.h>
#include <LedRouter.h>

namespace signal
{

  // -----------------------------------------------------------------------------------
  /// Calculate dim ramps
  // -----------------------------------------------------------------------------------
  void LedRouter::doRamps()
  {
    rte::Ifc_OnboardTargetIntensities::size_type pos = static_cast<rte::Ifc_OnboardTargetIntensities::size_type>(0);

    for (auto it = ramps_onboard.begin(); it != ramps_onboard.end(); it++)
    {
      rte::ifc_onboard_target_intensities::writeElement(pos, scale_16_8(it->step()));
      pos++;
    }
  }

  // -----------------------------------------------------------------------------------
  /// Server function: 
  /// - Initialize ramp for an output port with given intensity and slope / speed.
  /// - Step size of dim ramp is re-calculated.
  /// 
  /// @param tgt Output port
  /// @param intensity [0x0000 - 0x8000] Target intensity with 0x0000 = 0%, 0x8000 = 100%
  /// @param slope [(0x0000 - 0x8000)/ms] Slope / speed
  // -----------------------------------------------------------------------------------
  LedRouter::ret_type LedRouter::setIntensityAndSpeed(const target_type tgt, const intensity16_type intensity, const speed16_ms_type slope)
  {
    switch (tgt.type)
    {
    case cal::target_type::kOnboard:
    {
      if (ramps_onboard.check_boundary(tgt.idx))
      {
        ramps_onboard[tgt.idx].init_from_slope(intensity, slope, kCycleTime);
      }
    }
    break;
    case cal::target_type::kExternal:
      break;
    default:
      break;
    }

    return rte::ifc_base::OK;
  }

  // -----------------------------------------------------------------------------------
  /// Server function: Set the slope / speed to reach the target intensity but do not change the target intensity.
  /// 
  /// @param tgt Output port
  /// @param slope [(0x0000 - 0x8000)/ms] Slope / speed
  // -----------------------------------------------------------------------------------
  LedRouter::ret_type LedRouter::setSpeed(const target_type tgt, const speed16_ms_type slope)
  {
    switch (tgt.type)
    {
    case cal::target_type::kOnboard:
    {
      if (ramps_onboard.check_boundary(tgt.idx))
      {
        ramps_onboard[tgt.idx].set_slope(slope, kCycleTime);
      }
    }
    break;
    case cal::target_type::kExternal:
      break;
    default:
      break;
    }
    return rte::ifc_base::OK;
  }

  // -----------------------------------------------------------------------------------
  /// Server function: Set the target intensity but do not change the current speed.
  /// 
  /// @param tgt Output port
  /// @param intensity [0x0000 - 0x8000] Target intensity with 0x0000 = 0%, 0x8000 = 100%
  // -----------------------------------------------------------------------------------
  LedRouter::ret_type LedRouter::setIntensity(const target_type tgt, const intensity16_type intensity)
  {
    switch (tgt.type)
    {
    case cal::target_type::kOnboard:
    {
      if (ramps_onboard.check_boundary(tgt.idx))
      {
        ramps_onboard[tgt.idx].set_tgt(intensity);
      }
    }
    break;
    case cal::target_type::kExternal:
      break;
    default:
      break;
    }
    return rte::ifc_base::OK; 
  }

  // -----------------------------------------------------------------------------------
  /// Init at system start
  // -----------------------------------------------------------------------------------
  void LedRouter::init()
  {
  }

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void LedRouter::cycle()
  {
    doRamps();
  }
} // namespace signal

