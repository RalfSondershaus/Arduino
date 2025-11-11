
/**
 * @file App/Signal/Signal_cfg.h
 *
 * @brief Configuration header file for Signal class
 *
 * @note This file must be included in Signal.cpp only
 * @note All functions in this file are defined as inline to allow compile-time optimization
 *
 * @copyright Copyright 2025 Ralf Sondershaus
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

#ifndef APP_SIGNAL_CFG_H__
#define APP_SIGNAL_CFG_H__

#include <Std_Types.h>
#include <Cfg_Prj.h>
#include <Prj_Types.h>
#include <Cal/CalM_Types.h>
#include <Rte/Rte.h>
#include <Rte/Rte_Cfg_Cod.h>

namespace signal_cal
{
    /**
     * @brief Get the signal id which selects the change over time and aspects to be used
     * 
     * @param signal_idx Signal index in the array of signals (0 ... cfg::kNrSignals-1)
     * @return uint8 Signal id (eSignalNotUsed, eFirstBuiltInSignalId, ..., eFirstUserDefinedSignalID, ...)
     */
    inline uint8 get_signal_id(uint8 signal_idx)
    {
        return rte::get_cv(cal::cv::kSignalIDBase + signal_idx);
    }

    /** @brief Get the signal aspect for a user-defined signal ID
     * 
     * @param signal_id Signal id (user-defined)
     * @param cmd Command index (0 ... cfg::kNrSignalAspects-1)
     * @param aspect Output: signal aspect configuration
     */
    inline void get_signal_aspect(uint8 signal_id, uint8 cmd, struct signal::signal_aspect &aspect)
    {
       rte::sig::get_signal_aspect(signal_id, cmd, aspect);
    }
    /**
     * @brief Get the input configuration for the signal
     * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
     * @return uint8 CV value for input configuration
     */
    inline struct signal::input_cmd get_input_cmd(uint8 signal_idx)
    {
        struct signal::input_cal cal_input = rte::sig::get_input(signal_idx);
        struct signal::input_cmd input_cmd { 
            cal_input.type, /* type */
            signal_idx      /* index on RTE */
        };
        return input_cmd;
    }
    /**
     * @brief Get the input configuration for the signal
     * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
     * @return uint8 CV value for input configuration
     */
    inline struct signal::target get_first_output(uint8 signal_idx)
    {
        return rte::sig::get_first_output(signal_idx);
    }
} // namespace signal_cal

namespace signal_rte
{
    uint8 get_cmd(struct signal::input_cmd input)
    {
        return rte::ifc_rte_get_cmd::call(input);
    }
    /** 
     * Returns true if the cmd is valid 
     */
    inline bool isValid(uint8 cmd) noexcept 
    { 
        return cmd < cfg::kNrSignalAspects; 
    }
    void set_intensity_and_speed(const struct signal::target target, const util::intensity16 intensity, const util::speed16_ms speed)
    {
        rte::ifc_rte_set_intensity_and_speed::call(target, intensity, speed);
    }
    void set_intensity(const struct signal::target target, const util::intensity16 intensity)
    {
        rte::ifc_rte_set_intensity::call(target, intensity);
    }

}

#endif // APP_SIGNAL_CFG_H__
