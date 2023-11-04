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

#include <Std_Types.h>
#include <Rte/Rte_Type.h>
#include <Cal/CalM_Type.h>
#include <Util/Timer.h>

namespace signal
{
  
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class Signal
  {
  public:
    using cmd_type = rte::cmd_type;

    using aspect_type = cal::aspect_type;
    using size_type = util::array<aspect_type, cfg::kNrSignalAspects>::size_type;

    /// Access calibration data
    static bool cal_valid(const cal::signal_type * pCal) { return pCal != nullptr; }
    static cal::input_type cal_getInput(const cal::signal_type * pCal) { return pCal->input; }
    static cal::aspect_type cal_getAspect(const cal::signal_type * pCal, cmd_type cmd) { return pCal->aspects[static_cast<size_type>(cmd)]; }
    static cal::target_type cal_getTarget(const cal::signal_type * pCal, size_type idx) { return pCal->targets[idx]; }
    static uint8 cal_getChangeOverTime(const cal::signal_type * pCal) { return pCal->changeOverTime; }

    /// Returns true if the aspect is in its initial state (after startup). 
    /// Returns false otherwise (aspect has been used once at least).
    static bool isInitialState(aspect_type aspect) { return aspect.aspect == static_cast<uint8_t>(0U); }

  protected:
    /// Returns true if the cmd is valid
    bool isValid(cmd_type cmd) const noexcept { return cmd < cfg::kNrSignalAspects; }

    /// Current target aspect (can be 0 during transition, or final target aspect)
    aspect_type aspect_cur;
    /// Target aspect (final)
    aspect_type aspect_tgt;
    /// Change over time between transitions (time is used twice: for dim down and for dim up)
    util::MilliTimer changeOverTimer;
    /// transform unit [10 ms] to unit [1 ms]
    static uint16 scale_10ms_1ms(const rte::dimtime8_10ms_t time) noexcept { return static_cast<uint16>(10U * time); }

  public:

    Signal() : aspect_cur{ 0U, 0U }, aspect_tgt{ 0U, 0U }
    {}

    /// Returns the current aspect
    aspect_type getCurAspect() const noexcept { return aspect_cur; }

    /// Initialization
    void init();
    /// Check cmd and turn on current and target aspect if required
    rte::signal_intensity_type exec(const cal::signal_type * pCal);
  };

  // -----------------------------------------------------------------------------------
  /// Controls a list of cfg::kNrSignals signals. 
  /// Reads input commands from RTE, applies commands and writes aspects to RTE.
  // -----------------------------------------------------------------------------------
  class SignalHandler
  {
  public:
    using classified_values_array = rte::classified_values_array;
    using classified_value_type = classified_values_array::value_type;
    using classified_values_size_type = rte::classified_values_array::size_type;
    using cmd_type = rte::cmd_type;
    using signal_array_type = util::array<Signal, cfg::kNrSignals>;
    
  protected:
    /// the list of cfg::kNrSignals signals
    signal_array_type signals;

    bool cal_valid(const cal::signal_cal_type * pCal) const { return pCal != nullptr; }

  public:

    SignalHandler()
    {}

    /// RTE runables
    void init();
    void cycle();
  };
} // namespace signal

#endif // SIGNAL_H_