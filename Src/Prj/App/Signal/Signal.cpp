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
  /// Calculates target values and dim ramps for command cmd.
  /// Precondition: valid calibration data (pCal is valid).
  // -----------------------------------------------------------------------------------
  void Signal::exec(const cal::signal_type * pCal, const cmd_type cmd)
  {
    cal::aspect_type aspect_tmp;
    rte::dimtime8_10ms_t dimtime = static_cast<rte::dimtime8_10ms_t>(cal_getChangeOverTime(pCal));

    if (isValid(cmd))
    {
      const cal::aspect_type asp = cal_getAspect(pCal, cmd);
      if (aspect_tgt.aspect != asp.aspect)
      {
        // apply change over time only if current target aspect is not initial state
        if (aspect_tgt.aspect != static_cast<uint8_t>(0U))
        {
          changeOverTimer.start(scale_10ms_1ms(dimtime));
        }
        aspect_tgt = asp;
      }
    }

    if (!changeOverTimer.timeout())
    {
      aspect_tmp.aspect = 0U;
    }
    else
    {
      aspect_tmp = aspect_tgt;
    }

    if (aspect_cur.aspect != aspect_tmp.aspect)
    {
      aspect_cur = aspect_tmp;
    }
  }

  // -----------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------
  void SignalHandler::init()
  {
    bootstate = kUninitialized;
  }

  // -----------------------------------------------------------------------------------
  // -----------------------------------------------------------------------------------
  void SignalHandler::cycle()
  {
    size_t pos;
    rte::signal_intensity_type intensities_tgt;
    rte::intensity8_t intensity;

    const cal::signal_cal_type * pCal = rte::ifc_cal_signal::call();

    if (cal_valid(pCal))
    {
      auto calit = pCal->begin();
      for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
      {
        cmd_type cmd = rte::ifc_rte_get_cmd::call(calit->input);

        // switch on RED if this is called for the first time and no valid command is available
        if ((cmd == rte::kInvalidCmd) && (bootstate == kUninitialized))
        {
          cmd = 0; // 0 means RED by default
        }

        sigit->exec(calit, cmd);

        for (pos = static_cast<size_t>(0U); pos < static_cast<size_t>(cfg::kNrSignalTargets); pos++)
        {
          if (util::bits::test<uint8>(sigit->getCurAspect().aspect, pos))
          {
            intensity = static_cast<rte::intensity8_t>(rte::kIntensity8_100);
          }
          else
          {
            intensity = static_cast<rte::intensity8_t>(rte::kIntensity8_0);
          }
          // MSB of aspect is index 0 in target intensity array
          // LSB of aspect is index cfg::kNrSignalTargets-1 in target intensity array
          intensities_tgt.intensities[static_cast<size_t>((cfg::kNrSignalTargets - 1U) - pos)] = intensity;
          intensities_tgt.changeOverTime = Signal::cal_getChangeOverTime(calit);
          //lis.setIntensitySpeed(cal_getTarget(static_cast<size_t>((cfg::kNrSignalTargets - 1U) - pos)), intensity, dimtime);
        }

        rte::ifc_signal_target_intensities::writeElement(sigit - signals.begin(), intensities_tgt);

        calit++;
      }
    }

    // Initialization is performed when this function is called for the first time
    bootstate = kInitialized;
  }

} // namespace signal
