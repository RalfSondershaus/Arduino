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
  /// Initialize ramp for tgt with given intensity and time if intensity and time differ from current tamp target values.
  // -----------------------------------------------------------------------------------
  void LedRouter::setIntensitySpeed(const target_type tgt, const intensity8_type intensity, const dimtime8_10ms_type time)
  {
    switch (tgt.type)
    {
    case cal::target_type::kOnboard:
    {
      if (dimtimes_onboard.check_boundary(tgt.idx))
      {
        if ((dimtimes_onboard[tgt.idx] != time) || (ramps_onboard[tgt.idx].get_tgt() != scale_8_16(intensity)))
        {
          ramps_onboard[tgt.idx].init(scale_8_16(intensity), scale_10ms_1ms(time), kCycleTime);
          dimtimes_onboard[tgt.idx] = time;
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

    for (auto it = ramps_onboard.begin(); it != ramps_onboard.end(); it++)
    {
      rte::ifc_onboard_target_intensities::writeElement(pos, scale_16_8(it->step()));
      pos++;
    }
  }

  // -----------------------------------------------------------------------------------
  /// Sets calibration data
  // -----------------------------------------------------------------------------------
  //void LedRouter::set_cal(cal_const_pointer p)
  //{
  //  pCal = p;

  //  if (p != nullptr)
  //  {
  //    auto calit = p->begin();
  //    for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
  //    {
  //      sigit->set_cal(calit);
  //      calit++;
  //    }
  //  }
  //  else
  //  {
  //    for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
  //    {
  //      sigit->set_cal(nullptr);
  //    }
  //  }
  //}

  // -----------------------------------------------------------------------------------
  /// For the pos-th signal, map intensities and speed from RTE signal values to internal onboard or external values.
  /// @param pos [0 ... cfg::kNrSignals-1] signal id
  /// @param pCal pointer to calibration data of pos-th signal, must not be nullptr
  // -----------------------------------------------------------------------------------
  void LedRouter::mapSignal(size_type pos, const cal::signal_type * pCal)
  {
    rte::signal_intensity_type sigintensity;

    rte::ifc_signal_target_intensities::readElement(pos, sigintensity);
    auto calit = pCal->targets.begin();
    for (auto sigit = sigintensity.intensities.begin(); sigit != sigintensity.intensities.end(); sigit++)
    {
      setIntensitySpeed(*calit, *sigit, sigintensity.changeOverTime);
      calit++;
    }
  }

  // -----------------------------------------------------------------------------------
  /// For each signal, map intensities and speed from RTE signal values to internal onboard or external values.
  // -----------------------------------------------------------------------------------
  void LedRouter::mapSignals()
  {
    const cal::signal_cal_type * pCal = rte::ifc_cal_signal::call();
    //const cal::signal_cal_type * pCal = rte::Ifc_Cal_Signal::call();
    size_type pos;

    if (cal_signal_valid(pCal))
    {
      auto calit = pCal->begin();
      for (pos = static_cast<size_type>(0U); pos < rte::ifc_signal_target_intensities::size(); pos++)
      {
        mapSignal(pos, calit);
        calit++;
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
  void LedRouter::cycle()
  {
    mapSignals();
    doRamps();
  }
} // namespace signal

