/**
 * @file Signal/Signal.cpp
 *
 * @brief Defines a signal.
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
#include <Util/bitset.h>
#include <Signal.h>

namespace signal
{

  // -----------------------------------------------------------------------------------
  /// Initializes output if calibration data are available (set_cal needs to be called
  /// before).
  // -----------------------------------------------------------------------------------
  void Signal::init()
  {
  }

  // -----------------------------------------------------------------------------------
  /// - Gets command for given cal data
  /// - Calculates and returns target intensities and dim ramp for the command
  /// 
  /// Precondition: valid calibration data (pCal is valid).
  // -----------------------------------------------------------------------------------
  rte::signal_intensity_type Signal::exec(const cal::signal_type * pCal)
  {
    cal::aspect_type aspect_tmp;
    size_t pos;
    rte::signal_intensity_type intensities_tgt;
    constexpr size_t kNrSignalTargetsMaxIdx = cfg::kNrSignalTargets - 1U;

    rte::dimtime8_10ms_t dimtime = static_cast<rte::dimtime8_10ms_t>(cal_getChangeOverTime(pCal));

    cmd_type cmd = rte::ifc_rte_get_cmd::call(cal_getInput(pCal));

    // switch on RED if there hasn't been received a valid command since system start.
    if ((cmd == rte::kInvalidCmd) && (isInitialState(aspect_tgt)))
    {
      cmd = 0; // 0 means RED by default
    }

    // The target intensity is changed if the command is a valid command.
    // If the command is an invalid command, the (last) target intensity remains.
    if (isValid(cmd))
    {
      const cal::aspect_type asp = cal_getAspect(pCal, cmd);
      if (aspect_tgt.aspect != asp.aspect)
      {
        // apply change over time only if current target aspect is not initial state
        if (!isInitialState(aspect_tgt))
        {
          changeOverTimer.start(scale_10ms_1ms(dimtime));
        }
        aspect_tgt = asp;
      }
    }

    if (!changeOverTimer.timeout())
    {
      // aspect is changing, phase 1: dim down to zero intensity
      aspect_tmp.aspect = 0U;
    }
    else
    {
      // aspect is active or changing, phase 2: dim up to target intensities
      aspect_tmp = aspect_tgt;
    }

    if (aspect_cur.aspect != aspect_tmp.aspect)
    {
      aspect_cur = aspect_tmp;
    }

    // MSB of aspect is index 0 in target intensity array
    // LSB of aspect is index cfg::kNrSignalTargets-1 in target intensity array
    for (pos = 0U; pos < static_cast<size_t>(cfg::kNrSignalTargets); pos++)
    {
      if (util::bits::test<uint8>(aspect_cur.aspect, pos))
      {
        intensities_tgt.intensities[kNrSignalTargetsMaxIdx - pos] = rte::kIntensity8_100;
      }
      else
      {
        intensities_tgt.intensities[kNrSignalTargetsMaxIdx - pos] = rte::kIntensity8_0;
      }
      intensities_tgt.changeOverTime = dimtime;
    }

    return intensities_tgt;
  }

  // -----------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------
  void SignalHandler::init()
  {
  }

  // -----------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------
  void SignalHandler::cycle()
  {
    rte::signal_intensity_type intensities_tgt;

    const cal::signal_cal_type * pCal = rte::ifc_cal_signal::call();

    if (cal_valid(pCal))
    {
      auto calit = pCal->begin();
      for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
      {
        intensities_tgt = sigit->exec(calit);

        rte::ifc_signal_target_intensities::writeElement(sigit - signals.begin(), intensities_tgt);

        calit++;
      }
    }
  }

} // namespace signal
