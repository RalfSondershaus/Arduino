/**
 * @file Signal/Signal.h
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

#ifndef SIGNAL_H_
#define SIGNAL_H_

#include <Rte/Rte_Type.h>
#include <Cal/CalM_Type.h>
#include <LedIntensitySetter.h>

namespace signal
{
  
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class Signal
  {
  protected:
    using cmd_type = rte::cmd_type;
    using aspect_type = cal::aspect_type;
    using size_type = util::array<aspect_type, cfg::kNrSignalAspects>::size_type;

    const cal::signal_type * pCal;

    /// Access calibration data
    bool cal_valid() const { return pCal != nullptr; }
    cal::input_type cal_getInput() const { return pCal->input; }
    cal::aspect_type cal_getAspect(cmd_type cmd) const { return pCal->aspects[static_cast<size_type>(cmd)]; }
    cal::target_type cal_getTarget(size_type idx) const { return pCal->targets[idx]; }
    uint8 cal_getChangeOverTime() const { return pCal->changeOverTime; }
    bool isValid(cmd_type cmd) const noexcept { return cmd < cfg::kNrSignalAspects; }
  public:

    Signal() : pCal(nullptr)
    {}

    void exec(LedIntensitySetter& lis);
    void set_cal(const cal::signal_type * p);
  };

} // namespace signal

#endif // SIGNAL_H_