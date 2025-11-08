
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

/** Built-in signal outputs */
#define CAL_BUILT_IN_SIGNAL_OUTPUTS     \
    {                                   \
        5,                              \
        0b00011000, 0b00000000,         \
        0b00000100, 0b00000000,         \
        0b00000110, 0b00000000,         \
        0b00011001, 0b00000000,         \
        0b00011111, 0b00000000,         \
        0b00011111, 0b00000000,         \
        0b00011111, 0b00000000,         \
        0b00011111, 0b00000000,         \
        10, 10,                         \
        2,                              \
        0b00000001, 0b00000000,         \
        0b00000010, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        0b00000011, 0b00000000,         \
        10, 10                          \
    }

namespace signal_cal
{
    /** @brief Configuration data for signal outputs and aspects of a signal as defined by a signal id */
    struct signal_aspect
    {
        uint8 num_targets;                                ///< Number of outputs (LEDs)
        uint8 aspect; ///< A bit per output, max. 8 outputs: 0 = 0%, 1 = 100%, LSB = 1st output, MSB = 8th output
        uint8 blink;  ///< A bit per output, max. 8 outputs: 0 = no blinking, 1 = blinking
        uint8 change_over_time_10ms;                     ///< [10 ms] dim time if aspect changes
        uint8 change_over_time_blink_10ms;               ///< [10 ms] dim time for blinking effects
    };
    /**
     * @brief Built-in signal outputs
     */
    const uint8 ROM_CONST_VAR built_in_signal_outputs[cal::cv::kSignalLength * cfg::kNrBuiltInSignals] = CAL_BUILT_IN_SIGNAL_OUTPUTS;

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

    /**
     * @brief Check if the signal id is a valid user-defined signal id
     * 
     * @param signal_id Signal id to be checked
     * @return true signal id is a valid user-defined signal id
     * @return false signal id is not a valid user-defined signal id
     */
    bool is_built_in(uint8 signal_id) noexcept 
    { 
        return (signal_id >= cal::signal::kFirstBuiltInSignalID) &&
               (signal_id < cal::signal::kFirstBuiltInSignalID + cfg::kNrBuiltInSignals);
    }

    /**
     * @brief Check if the signal id is a valid built-in signal id
     * 
     * @param signal_id Signal id to be checked
     * @return true signal id is a valid built-in signal id
     * @return false signal id is not a valid built-in signal id
     */
    bool is_user_defined(uint8 signal_id) noexcept 
    { 
        return (signal_id >= cal::signal::kFirstUserDefinedSignalID) &&
               (signal_id < cal::signal::kFirstUserDefinedSignalID + cfg::kNrUserDefinedSignals);
    }

    /**
     * @brief Returns the zero based index of signal id to be used for array indexing
     * 
     * @param id signal id
     * @return uint8 zero based index of the signal id
     * 
     * @note Use for built in signal ids only.
     */
    uint8 zero_based_built_in(uint8 signal_id)  { return signal_id - cal::signal::kFirstBuiltInSignalID; }

    /**
     * @brief Returns the zero based index of signal id to be used for array indexing
     * 
     * @param id signal id
     * @return uint8 uint8 zero based index of the signal id
     * 
     * @note Use for built in signal ids only.
     */
    uint8 zero_based_user_defined(uint8 signal_id) { return signal_id - cal::signal::kFirstUserDefinedSignalID; }

    /** @brief Get the signal aspect for a user-defined signal ID
     * 
     * @param signal_id Signal id (user-defined)
     * @param cmd Command index (0 ... cfg::kNrSignalAspects-1)
     * @param aspect Output: signal aspect configuration
     */
    inline void get_signal_aspect(uint8 signal_id, uint8 cmd, struct signal_aspect &aspect)
    {
        if (is_user_defined(signal_id))
        {
            uint16 index = zero_based_user_defined(signal_id);
            index = cal::cv::kUserDefinedSignalBase + index * cal::cv::kSignalLength;
            aspect.num_targets = util::bits::masked_shift(
                rte::get_cv(index++),
                cal::signal::bitmask::kNumberOfOutputs,
                cal::signal::bitshift::kNumberOfOutputs);
            aspect.aspect = rte::get_cv(index + 2U*cmd);
            aspect.blink = rte::get_cv(index + 2U*cmd + 1U); index += 2U*cfg::kNrSignalAspects;
            aspect.change_over_time_10ms = rte::get_cv(index++);
            aspect.change_over_time_blink_10ms = rte::get_cv(index++);
        }
        else if (is_built_in(signal_id))
        {
            uint16 index = zero_based_built_in(signal_id);
            aspect.num_targets = util::bits::masked_shift(
                ROM_READ_BYTE(&built_in_signal_outputs[index++]),
                cal::signal::bitmask::kNumberOfOutputs,
                cal::signal::bitshift::kNumberOfOutputs);
            aspect.aspect = ROM_READ_BYTE(&built_in_signal_outputs[index + 2U*cmd]);
            aspect.blink = ROM_READ_BYTE(&built_in_signal_outputs[index + 2U*cmd + 1U]); index += 2U*cfg::kNrSignalAspects;
            aspect.change_over_time_10ms = ROM_READ_BYTE(&built_in_signal_outputs[index++]);
            aspect.change_over_time_blink_10ms = ROM_READ_BYTE(&built_in_signal_outputs[index++]);
        }
        else
        {
            aspect.num_targets = 0U;
            aspect.aspect = 0U;
            aspect.blink = 0U;
            aspect.change_over_time_10ms = 0U;
            aspect.change_over_time_blink_10ms = 0U;
        }
    }
    /**
     * @brief Get the input configuration for the signal
     * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
     * @return uint8 CV value for input configuration
     */
    inline struct signal::input get_input(uint8 signal_idx)
    {
        struct signal::input input { 
            signal_idx, /* pin */
            util::bits::masked_shift(
                rte::get_cv(cal::cv::kSignalInputBase + signal_idx),
                cal::signal::bitmask::kInputType,
                cal::signal::bitshift::kInputType) /* type */
        };
        return input;
    }
    /**
     * @brief Get the input configuration for the signal
     * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
     * @return uint8 CV value for input configuration
     */
    inline struct signal::target get_first_output(uint8 signal_idx)
    {
        struct signal::target output = rte::get_cv(cal::cv::kSignalFirstOutputBase + signal_idx);
        return output;
    }
} // namespace signal_cal

namespace signal_rte
{
    uint8 get_cmd(struct signal::input input)
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
