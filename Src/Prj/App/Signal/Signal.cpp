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
  void Signal::exec(const cal::signal_type * pCal)
  {
    cal::aspect_type aspect_cur;
    size_t pos;
    intensity16_type intensity;

    rte::dimtime8_10ms_t dimtime = static_cast<rte::dimtime8_10ms_t>(cal_getChangeOverTime(pCal));

    cmd_type cmd = rte::ifc_rte_get_cmd::call(cal_getInput(pCal));

    // switch on RED if a valid command hasn't been received since system start.
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
      aspect_cur.aspect = 0U;
    }
    else
    {
      // aspect is active or changing, phase 2: dim up to target intensities
      aspect_cur = aspect_tgt;
    }

    // write intensity and speed to RTE
    constexpr size_t kSignalTargetIndexMax = static_cast<size_t>(cfg::kNrSignalTargets) - 1U;
    for (pos = 0U; pos <= kSignalTargetIndexMax; pos++)
    {
      // MSB of aspect is index 0 in target intensity array
      // LSB of aspect is index cfg::kNrSignalTargets-1 in target intensity array
      if (util::bits::test<uint8>(aspect_cur.aspect, kSignalTargetIndexMax - pos))
      {
        intensity = rte::kIntensity16_100;
      }
      else
      {
        intensity = rte::kIntensity16_0;
      }

      // update speed only if dim time has changed in order to minimize calculation time
      if (dimtime != last_dim_time)
      {
        // dimtime [10 ms]
        // speed [(0x0000 ... 0x8000) / ms]
        // calculate speed for ramp from 0x0000 (0%) to 0x8000 (100%) within dimtime
        const speed16_ms_type speed = rte::kIntensity16_100 / scale_10ms_1ms(dimtime);

        rte::ifc_rte_set_intensity_and_speed::call(cal_getTarget(pCal, pos), intensity, speed);
      }
      else
      {
        rte::ifc_rte_set_intensity::call(cal_getTarget(pCal, pos), intensity);
      }
    }

    last_dim_time = dimtime;
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
    const cal::signal_cal_type * pCal = rte::ifc_cal_signal::call();

    if (cal_valid(pCal))
    {
      auto calit = pCal->begin();
      for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
      {
        sigit->exec(calit);
        calit++;
      }
    }
  }

} // namespace signal
