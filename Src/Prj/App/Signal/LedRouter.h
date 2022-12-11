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
#include <LedIntensitySetter.h>
#include <Signal.h>

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// 
  // -----------------------------------------------------------------------------------
  class LedRouter : public LedIntensitySetter
  {
  public:
    using classified_values_array = rte::classified_values_array;
    using classified_value_type = classified_values_array::value_type;
    using classified_values_size_type = rte::classified_values_array::size_type;
    using cmd_type = rte::cmd_type;
    typedef util::ramp<uint16> ramp_type;
    typedef util::array<ramp_type, cfg::kNrOnboardTargets> ramp_onboard_array_type;
    typedef util::array<ramp_type, cfg::kNrExternalTargets> ramp_external_array_type;
    typedef util::array<dimtime8_10ms_t, cfg::kNrOnboardTargets> dimtime_onboard_array_type;
    typedef util::array<dimtime8_10ms_t, cfg::kNrExternalTargets> dimtime_external_array_type;
    typedef util::array<Signal, cfg::kNrSignals> signal_array_type;

  protected:

    ramp_onboard_array_type ramps_onboard;
    dimtime_onboard_array_type dimtimes_onboard;
    ramp_external_array_type ramps_external;
    dimtime_external_array_type dimtimes_external;
    signal_array_type signals;

    const cal::signal_array * pCal;

    /// Calculate new target intensities
    void handleSignals();

    /// Caclulate ramps
    void doRamps();

    /// Access calibration data
    bool cal_valid() const noexcept { return pCal != nullptr; }

  public:
    LedRouter() : pCal(nullptr) {}

    /// Runables
    void init();
    void cycle10();

    /// Server runable: set calibration values
    void set_cal(const cal::signal_array * p);

    /// Interface LedIntensitySetter
    virtual void setIntensitySpeed(target_type tgt, intensity8_t intensity, dimtime8_10ms_t time) override;
  };
} // namespace signal

#endif // LED_ROUTER_H_
