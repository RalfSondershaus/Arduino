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
    using speed16_ms_type = rte::speed16_ms_t;
    using ret_type = rte::ret_type;
    using cmd_type = rte::cmd_type;
    using size_type = rte::Ifc_SignalTargetIntensities::size_type;
    using target_type = cal::target_type;

    typedef util::ramp<intensity16_type> ramp_type;
    typedef util::array<ramp_type, cfg::kNrOnboardTargets> ramp_onboard_array_type;
    typedef util::array<ramp_type, cfg::kNrExternalTargets> ramp_external_array_type;

  protected:

    ramp_onboard_array_type ramps_onboard;
    ramp_external_array_type ramps_external;

    /// Maximal intensity value (0x8000 = 100%) which uses 16 bits. Values less than kIntensity16Max use 15 bits only.
    static constexpr intensity16_type kIntensity16Max = 0x8000U;

    /// Scale from [0x0000, 0x8000] to [0, 255]
    static intensity8_type scale_16_8(const intensity16_type intensity) noexcept { return (intensity >= kIntensity16Max) ? (255) : (static_cast<intensity8_type>(intensity / 128U)); }

    static constexpr uint8 kCycleTime = 10U;

    /// Caclulate ramps
    void doRamps();

  public:
    LedRouter() {}

    /// Runables
    void init();
    void cycle();

    /// Server function: Initialize ramp for tgt with given intensity and slope / speed. Dim ramp is re-started.
    ret_type setIntensityAndSpeed(const target_type tgt, const intensity16_type intensity, const speed16_ms_type slope);
    /// Server function: Set the slope / speed to reach the target intensity but do not change the target intensity.
    /// @param tgt Output port
    /// @param slope [%/ms] Slope / speed
    ret_type setSpeed(const target_type tgt, const speed16_ms_type slope);
    /// Server function: Set the target intensity but do not change the current speed.
    ret_type setIntensity(const target_type tgt, const intensity16_type intensity);

  };
} // namespace signal

#endif // LED_ROUTER_H_
