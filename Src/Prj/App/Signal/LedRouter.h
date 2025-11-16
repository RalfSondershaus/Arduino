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

#include <Rte/Rte_Types.h>
#include <Util/Intensity.h>
#include <Util/Ramp.h>
#include <Util/Array.h>

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// Routes target intensities from RTE to physical output channels (internal and external).
  /// Examples for internal physical output channels are PWM output pins or digital
  /// output pins.
  /// Examples for external physical output channels are external drivers which
  /// are connected via SPI or external shift register drivers.
  ///
  /// Currently, only internal physical channels are supported.
  ///
  /// For each physical output channel, set the target intensity and the slope to
  /// reach this target intensity. LedRouter will calculate the dim ramp.
  ///
  /// Runables:
  /// - init (to be called once at startup)
  /// - cycle (cyclic 10 ms, to be called cyclicly)
  ///
  /// Input: RTE server functions
  /// - setIntensityAndSpeed
  /// - setSpeed
  /// - setIntensity
  ///
  /// Output: RTE SR port
  /// - rte::ifc_onboard_target_duty_cycles (rte::Ifc_OnboardTargetDutyCycles)
  // -----------------------------------------------------------------------------------
  class LedRouter
  {
  public:
    using intensity8_type = util::intensity8;
    using intensity8_255_type = util::intensity8_255;
    using intensity16_type = util::intensity16;
    using speed16_ms_type = util::speed16_ms;
    using ramp_base_type = intensity16_type::base_type;
    using ret_type = rte::ret_type;

    typedef util::ramp<ramp_base_type> ramp_type;
    typedef util::array<ramp_type, cfg::kNrOnboardTargets> ramp_onboard_array_type;
    typedef util::array<ramp_type, cfg::kNrExternalTargets> ramp_external_array_type;

  protected:

    ramp_onboard_array_type ramps_onboard;
    ramp_external_array_type ramps_external;

    static constexpr uint8 kCycleTime = 10U;

    /// Caclulate ramps
    void doRamps();

  public:
    /// @brief construct
    LedRouter() {}

    // Runables
    /// @brief Init runable
    /// @note Average run time 109 usec @ATmega2560 @16 MHz with -Os
    /// @note Average run time 97 usec @ATmega2560 @16 MHz with -O3
    void init();
    /// @brief Cyclic runable
    /// @note Average run time 567 usec if one of 54 ramps is active @ATmega2560 @16 MHz with -Os
    /// @note Average run time 552 usec if one of 54 ramps is active @ATmega2560 @16 MHz with -O3
    /// @note Average run time 630 usec if one of 54 ramps is active @ATmega2560 @16 MHz with -Os
    /// @note Average run time 625 usec if one of 54 ramps is active @ATmega2560 @16 MHz with -Os
    void cycle();

    /// Server function: Set the target intensity for the given output target port. Use a dim ramp with given slope to
    /// reach the target intensity.
    /// @param tgt Target output port
    /// @param intensity Target intensity
    /// @param slope Target slope (speed) to reach the target intensity
    /// @return rte::ifc_base::OK
    ret_type setIntensityAndSpeed(const struct signal::target tgt, const intensity16_type intensity, const speed16_ms_type slope);
    /// Server function: Set the slope / speed to reach the target intensity but do not change the target intensity.
    /// @param tgt Output port
    /// @param slope [%/ms] Slope / speed
    ret_type setSpeed(const struct signal::target tgt, const speed16_ms_type slope);
    /// Server function: Set the target intensity but do not change the current speed.
    ret_type setIntensity(const struct signal::target tgt, const intensity16_type intensity);

  };
} // namespace signal

#endif // LED_ROUTER_H_
