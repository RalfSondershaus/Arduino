
/**
 * @file App/Signal/Signal_cfg.h
 *
 * @brief Configuration header file for Signal class. Implements inline functions to access
 *        interfaces for calibration and RTE.
 *
 * @note This file must be included in Signal.cpp only
 * @note All functions in this file are defined as inline to allow compile-time optimization
 *
 * @copyright Copyright 2025 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
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

    /** 
     * @brief Get the signal aspect for the signal
     * 
     * @note The function name differs from CalM::get_signal_aspect to avoid confusion.
     * CalM::get_signal_aspect uses the Signal ID signal_id as first parameter, this function 
     * uses signal_idx.
     * 
     * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
     * @param cmd Command index (0 ... cfg::kNrSignalAspects-1)
     * @param aspect Output: signal aspect configuration
     */
    inline void get_signal_aspect_for_idx(uint8 signal_idx, uint8 cmd, struct signal::signal_aspect &aspect)
    {
       rte::sig::get_signal_aspect_for_idx(signal_idx, cmd, aspect);
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
    /**
     * @brief Return true if the output pin order is inverse, e.g. pins 13, 12, 11, 10 instead 
     *        of 13, 14, 15, 16
     * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
     * @return true Output pin order is inverse
     * @return false Output pin order is normal
     */
    static inline bool is_output_pin_order_inverse(uint8 signal_idx) { return rte::sig::is_output_pin_order_inverse(signal_idx); }
    /**
     * @brief Returns the output pin step size (1 or 2).
     * 
     * Step size 1, increasing order means, e.g., 13, 14, 15, 16
     * Step size 2, increasing order means, e.g., 13, 15, 17, 19
     * Step size 1, decreasing (inverse) order means, e.g., 13, 12, 11, 10
     * Step size 2, decreasing (inverse) order means, e.g., 13, 11, 9, 7
     * 
     * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
     * @return 1 or 2 Output pin step size
     */
    static inline uint8 get_output_pin_step_size(uint8 signal_idx) { return rte::sig::get_output_pin_step_size(signal_idx); }
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
