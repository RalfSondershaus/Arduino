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

#include <Signal.h>
#include <Rte/Rte.h>
#include <Util/bitset.h>

namespace signal
{

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void Signal::set_cal(const cal::signal_type * p)
  {
    pCal = p;
  }

  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  void Signal::exec(LedIntensitySetter& lis)
  {
    size_t pos;
    rte::intensity8_t intensity;
    rte::dimtime8_10ms_t dimtime = static_cast<rte::dimtime8_10ms_t>(cal_getChangeOverTime());

    if (cal_valid())
    {
      const cmd_type cmd = rte::ifc_rte_get_cmd.call(cal_getInput());
      if (isValid(cmd))
      {
        const cal::aspect_type aspect = cal_getAspect(cmd);
        for (pos = 0; pos < static_cast<size_t>(cfg::kNrSignalTargets); pos++)
        {
          if (util::bits::test<uint8>(aspect.aspect, pos))
          {
            intensity = static_cast<rte::intensity8_t>(100U);
          }
          else
          {
            intensity = static_cast<rte::intensity8_t>(0U);
          }
          lis.setIntensitySpeed(cal_getTarget(pos), intensity, dimtime);
        }
      }
    }
  }
} // namespace signal
