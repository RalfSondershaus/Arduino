/**
 * @file Signal/LedRouter.h
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

#ifndef LED_ROUTER_H_
#define LED_ROUTER_H_

#include <Rte/Rte_Type.h>
#include <Util/Algorithm.h>
#include <Util/Array.h>
#include <Cal/CalM_Type.h>
#include <Signal.h>

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class LedRouter
  {
  public:
    using intensity8_type = rte::intensity8_t;
    using intensity16_type = rte::intensity16_t;
    using dimtime8_10ms_type = rte::dimtime8_10ms_t;
    using classified_values_array = rte::classified_values_array;
    using classified_value_type = classified_values_array::value_type;
    using classified_values_size_type = rte::classified_values_array::size_type;
    using cmd_type = rte::cmd_type;
    using size_type = rte::Ifc_SignalTargetIntensities::size_type;
    using target_type = cal::target_type;

    typedef util::ramp<intensity16_type> ramp_type;
    typedef util::array<ramp_type, cfg::kNrOnboardTargets> ramp_onboard_array_type;
    typedef util::array<ramp_type, cfg::kNrExternalTargets> ramp_external_array_type;
    typedef util::array<dimtime8_10ms_type, cfg::kNrOnboardTargets> dimtime_onboard_array_type;
    typedef util::array<dimtime8_10ms_type, cfg::kNrExternalTargets> dimtime_external_array_type;
    //typedef util::array<Signal, cfg::kNrSignals> signal_array_type;
    //typedef const cal::signal_cal_type * cal_const_pointer;


  protected:

    ramp_onboard_array_type ramps_onboard;
    ramp_external_array_type ramps_external;
    dimtime_onboard_array_type dimtimes_onboard;
    dimtime_external_array_type dimtimes_external;

    /// Access calibration data
    static bool cal_signal_valid(const cal::signal_cal_type * pCal) noexcept { return pCal != nullptr; }

    /// Scaling
    static intensity16_type scale_8_16(const intensity8_type intensity) noexcept { return static_cast<intensity16_type>(256U * intensity); }
    static intensity8_type scale_16_8(const intensity16_type intensity) noexcept { return static_cast<intensity8_type>(intensity / 256U); }
    static uint16 scale_10ms_1ms(const dimtime8_10ms_type time) noexcept { return static_cast<uint16>(10U * time); }

    static constexpr uint8 kCycleTime = 10U;

    /// Caclulate ramps
    void doRamps();
    void mapSignals();
    void mapSignal(size_type pos, const cal::signal_type * pCal);

    /// Initialize ramp for tgt with given intensity and time if intensity and time differ from current tamp target values.
    void setIntensitySpeed(const target_type tgt, const intensity8_type intensity, const dimtime8_10ms_type time);

  public:
    LedRouter() {}

    /// Runables
    void init();
    void cycle();

  };
} // namespace signal

#endif // LED_ROUTER_H_
