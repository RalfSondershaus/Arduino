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
#include <Cfg_Prj.h>
#include <Util/Array.h>
#include <Util/Timer.h>

namespace signal
{
  
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class Signal
  {
  protected:
    /// Target aspect (final)
    uint8 aspect_tgt;
    /// last valid dim time (from coding data)
    uint8 last_dim_time_10ms;
    /// Change over time between transitions (time is used twice: for dim down and for dim up)
    util::MilliTimer changeOverTimer;
    /** Signal index (0 ... cfg::kNrSignals-1) */
    uint8 signal_idx;

    /// transform unit [10 ms] to unit [1 ms]
    static uint16 scale_10ms_1ms(const uint8 time_10ms) noexcept { return static_cast<uint16>(10U * time_10ms); }

  public:

    Signal() = default;

    /**
     * @brief Initialize to default values
     * 
     * @param idx Signal index (0 ... cfg::kNrSignals-1)
     */
    void init(uint8_t idx)
    {
        signal_idx = idx;
        aspect_tgt = 0U;
        last_dim_time_10ms = 0U;
        changeOverTimer.start(0U);
    }
    /// Check cmd and turn on current and target aspect if required
    void exec();
  };

  // -----------------------------------------------------------------------------------
  /// Controls a list of cfg::kNrSignals signals. 
  /// Reads input commands from RTE, applies commands and writes aspects to RTE.
  // -----------------------------------------------------------------------------------
  class SignalHandler
  {
  public:
    /**
     * @brief Type definition for the array of signals
     */
    using signal_array_type = util::array<Signal, cfg::kNrSignals>;
    
  protected:
    /**
     * @brief The array of signals.
     * 
     * Each signal can be accessed via its index (0 ... cfg::kNrSignals-1).
     */
    signal_array_type signals;

  public:

    /**
     * @brief Construct a new Signal Handler object
     */
    SignalHandler()
    {}

    /**
     * @defgroup RTE cyclic runables
     * @{
     */
    /**
     * @brief Initialize all signals at system start
     */
    void init();
    /**
     * @brief Cyclic function to be called every cycle, e.g., every 10 ms
     * 
     * This function reads the RTE input commands, applies them to the signals,
     * and writes the resulting signal aspects to the RTE outputs.
     * The signal aspects include the target intensity and dimming speed.
     */
    void cycle();
    /** @} */

    /**
     * @defgroup RTE server runables
     * @{
     */
    /** @} */
  };
} // namespace signal

#endif // SIGNAL_H_