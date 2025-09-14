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
    using dimtime8_10ms_type = rte::dimtime8_10ms_t;
    using intensity16_type = rte::intensity16;
    using speed16_ms_type = rte::speed16_ms_t;

    /// Access calibration data
    static bool cal_valid(const cal::signal_type * pCal) { return pCal != nullptr; }
    static uint8 cal_getNrTargets(const cal::signal_type * pCal) 
    { 
        return (pCal->output_ptr) ? (pCal->output_ptr->num_targets) : (0); 
    }
    static cal::input_type cal_getInput(const cal::signal_type * pCal) { return pCal->input; }
    static cal::aspect_type cal_getAspect(const cal::signal_type * pCal, cmd_type cmd) 
    { 
        return (pCal->output_ptr) ? 
               (pCal->output_ptr->aspects[static_cast<size_type>(cmd)]) :
               (cal::aspect_type{}); 
    }
    static cal::target_type cal_getFirstTarget(const cal::signal_type * pCal) 
    { 
        return pCal->first_target;
    }
    static uint8 cal_getChangeOverTime(const cal::signal_type * pCal) 
    { 
        return (pCal->output_ptr) ? (pCal->output_ptr->change_over_time) : (0);
    }

    /// Returns true if the aspect is in its initial state (after startup). 
    /// Returns false otherwise (aspect has been used once at least).
    static bool isInitialState(aspect_type aspect) { return aspect.aspect == static_cast<uint8_t>(0U); }

  protected:
    /// Returns true if the cmd is valid
    bool isValid(cmd_type cmd) const noexcept { return cmd < cfg::kNrSignalAspects; }

    /// Target aspect (final)
    aspect_type aspect_tgt;
    /// last valid dim time (from coding data)
    dimtime8_10ms_type last_dim_time;
    /// Change over time between transitions (time is used twice: for dim down and for dim up)
    util::MilliTimer changeOverTimer;

    /// transform unit [10 ms] to unit [1 ms]
    static uint16 scale_10ms_1ms(const rte::dimtime8_10ms_t time) noexcept { return static_cast<uint16>(10U * time); }

  public:

    Signal() : aspect_tgt{ 0U, 0U }, last_dim_time(0U)
    {}

    /// Initialize to default values of default constructor
    void init();
    /// Check cmd and turn on current and target aspect if required
    void exec(const cal::signal_type * pCal);
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