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

namespace signal
{
 
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void LedRouter::handleSignals()
  {
    for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
    {
      sigit->exec(*this);
    }
  }

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void LedRouter::setIntensitySpeed(target_type tgt, intensity8_t intensity, dimtime8_10ms_t time)
  {
    switch (tgt.type)
    {
    case cal::target_type::kOnboard:
    {
      if (dimtimes_onboard.check_boundary(tgt.idx))
      {
        if (dimtimes_onboard.at(tgt.idx) != time)
        {
          ramps_onboard.at(tgt.idx).init(255U*intensity, 10U * time, 10U);
          dimtimes_onboard.at(tgt.idx) = time;
        }
      }
    }
    break;
    case cal::target_type::kExternal:
      break;
    default:
      break;
    }
  }

  // -----------------------------------------------------------------------------------
  /// Calculate dim ramps
  // -----------------------------------------------------------------------------------
  void LedRouter::doRamps()
  {
    rte::Ifc_OnboardTargetIntensities::size_type pos = static_cast<rte::Ifc_OnboardTargetIntensities::size_type>(0);

    for (auto it = ramps_onboard.begin(); it = ramps_onboard.end(); it++)
    {
      rte::ifc_onboard_target_intensities.writeElement(pos, it->step() / 255U);
      pos++;
    }
  }

  // -----------------------------------------------------------------------------------
  /// Sets calibration data
  // -----------------------------------------------------------------------------------
  void LedRouter::set_cal(const cal::signal_array * p)
  {
    pCal = p;

    if (p != nullptr)
    {
      auto calit = p->begin();
      for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
      {
        sigit->set_cal(calit);
        calit++;
      }
    }
    else
    {
      for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
      {
        sigit->set_cal(nullptr);
      }
    }
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
  void LedRouter::cycle10()
  {
    handleSignals();
    doRamps();
  }
} // namespace signal

