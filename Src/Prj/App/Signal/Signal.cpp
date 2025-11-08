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
#include <Signal_cfg.h>
#include <Util/bitset.h>
#include <Util/intensity.h>

namespace signal
{
    /// Returns true if the aspect is in its initial state (after startup). 
    /// Returns false otherwise (aspect has been used once at least).
    static inline bool isInitialState(uint8 aspect) noexcept { return aspect == 0U; }

    // -----------------------------------------------------------------------------------
    /// - Gets command for given cal data
    /// - Calculates and returns target intensities and dim ramp for the command
    ///
    /// Precondition: valid calibration data (pCal is valid).
    // -----------------------------------------------------------------------------------
    void Signal::exec()
    {
        uint8 aspect_cur;
        size_t pos;
        util::intensity16 intensity;
        struct signal_cal::signal_aspect signal_asp;

        const uint8 signal_id = signal_cal::get_signal_id(signal_idx);

        uint8 cmd = signal_rte::get_cmd(signal_cal::get_input(signal_idx));

        // switch on RED if a valid command hasn't been received since system start.
        if ((!signal_rte::isValid(cmd)) && (isInitialState(aspect_tgt)))
        {
            cmd = 0; // 0 means RED by default
        }

        signal_cal::get_signal_aspect(signal_id, cmd, signal_asp);

        if (signal_asp.change_over_time_10ms == 0)
        {
            signal_asp.change_over_time_10ms = 1;
        }

        // The target intensity is changed if the command is a valid command.
        // If the command is an invalid command, the (last) target intensity remains.
        if (signal_rte::isValid(cmd))
        {
            if (aspect_tgt != signal_asp.aspect)
            {
                // apply change over time only if current target aspect is not initial state,
                // so the current aspect is a valid aspect which needs to dim down before
                // the new aspect can dim up.
                if (!isInitialState(aspect_tgt))
                {
                    changeOverTimer.start(scale_10ms_1ms(signal_asp.change_over_time_10ms));
                }
                aspect_tgt = signal_asp.aspect;
            }
        }

        if (!changeOverTimer.timeout())
        {
            // aspect is changing, phase 1: dim down to zero intensity
            aspect_cur = 0U;
        }
        else
        {
            // aspect is active or changing, phase 2: dim up to target intensities
            aspect_cur = aspect_tgt;
        }

        // write intensity and speed to RTE
        struct signal::target tgt = signal_cal::get_first_output(signal_idx);
        for (pos = 0U; pos < signal_asp.num_targets; pos++)
        {
            // MSB of aspect is index 0 in target intensity array
            // LSB of aspect is index cfg::kNrSignalTargets-1 in target intensity array
            if (util::bits::test<uint8>(aspect_cur, (signal_asp.num_targets - 1) - pos))
            {
                intensity = util::kIntensity16_100;
            }
            else
            {
                intensity = util::kIntensity16_0;
            }

            // update speed only if dim time has changed in order to minimize calculation time
            if (signal_asp.change_over_time_10ms != last_dim_time_10ms)
            {
                // aspect.change_over_time_10ms [10 ms]
                // Division by zero is not possible because aspect.change_over_time_10ms is checked above.
                // speed [(0x0000 ... 0x8000) / ms]
                // calculate speed for ramp from 0x0000 (0%) to 0x8000 (100%) within aspect.change_over_time_10ms
                const util::speed16_ms speed = util::kIntensity16_100 / scale_10ms_1ms(signal_asp.change_over_time_10ms);
                // log << " (" << cal_getTarget(pCal, pos).idx << ": " << intensity << ")";
                //  boundary check is performed in signal_rte::ifc_rte_set_intensity_and_speed
                signal_rte::set_intensity_and_speed(tgt, intensity, speed);
                tgt.pin++;
            }
            else
            {
                // log << " (" << cal_getTarget(pCal, pos).idx << ": " << intensity << ")";
                //  boundary check is performed in rte::ifc_rte_set_intensity
                signal_rte::set_intensity(tgt, intensity);
                tgt.pin++;
            }
        }

        last_dim_time_10ms = signal_asp.change_over_time_10ms;
    }

    // -----------------------------------------------------------------------------------
    /// Initialize to default values of default constructor
    // -----------------------------------------------------------------------------------
    void SignalHandler::init()
    {
        for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
        {
            sigit->init(sigit - signals.begin());
        }
    }

    /**
     * @brief 
     * 
     */
    void SignalHandler::cycle()
    {
        for (auto sigit = signals.begin(); sigit != signals.end(); sigit++)
        {
            sigit->exec();
        }
    }

} // namespace signal
