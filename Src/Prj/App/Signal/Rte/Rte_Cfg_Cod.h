/**
 * @file Rte_Cfg_Cod.h
 *
 * @author Ralf Sondershaus
 *
 * @brief RTE for calibration data of Signal.
 *
 * This file doesn't have include guards because it is included several times in a row.
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

#ifndef RTE_CFG_COD_H
#define RTE_CFG_COD_H

#include <Std_Types.h>
#include <Prj_Types.h>
#include <Cal/CalM.h>

namespace rte
{
    extern cal::CalM calm;

    static inline uint8 get_cv(uint16 cv)               { return calm.get_cv(cv); }
    static inline void set_cv(uint16 cv_id, uint8 val)  { return calm.set_cv(cv_id, val); }
    static inline bool is_cv_id_valid(uint16 cv_id)     { return calm.is_cv_id_valid(cv_id); }

    static inline bool ifc_cal_set_defaults()           { return calm.set_defaults(); }

    /**
     * @brief Namespace for signal-related RTE functions
     * 
     * @note Use namespace name 'sig' to avoid confusion with namespace 'signal'
     */
    namespace sig
    {
        /**
         * @brief Get the signal id which selects the change over time and aspects to be used
         *
         * @param signal_idx Signal index in the array of signals (0 ... cfg::kNrSignals-1)
         * @return uint8 Signal id (eSignalNotUsed, eFirstBuiltInSignalId, ..., eFirstUserDefinedSignalID, ...)
         */
        static inline uint8 get_signal_id(uint8 signal_idx) { return get_cv(cal::cv::kSignalIDBase + signal_idx);  }
        /**
         * @brief Check if the signal id is a valid user-defined signal id
         * 
         * @param signal_id Signal id to be checked
         * @return true signal id is a valid user-defined signal id
         * @return false signal id is not a valid user-defined signal id
         */
        static inline bool is_built_in(uint8 signal_id) noexcept { return calm.is_built_in(signal_id); }

        /**
         * @brief Check if the signal id is a valid built-in signal id
         * 
         * @param signal_id Signal id to be checked
         * @return true signal id is a valid built-in signal id
         * @return false signal id is not a valid built-in signal id
         */
        static inline bool is_user_defined(uint8 signal_id) noexcept { return calm.is_user_defined(signal_id); }

        /** @brief Get the signal aspect for a user-defined signal ID
         * 
         * @param signal_id Signal id (user-defined)
         * @param cmd Command index (0 ... cfg::kNrSignalAspects-1)
         * @param aspect Output: signal aspect configuration
         */
        static inline void get_signal_aspect(uint8 signal_id, uint8 cmd, struct signal::signal_aspect& aspect)
        {
            calm.get_signal_aspect(signal_id, cmd, aspect);
        }

        /**
         * @brief Get the input configuration for the signal
         * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
         * @return struct signal::input_cal input configuration
         */
        static inline struct signal::input_cal get_input(uint8 signal_idx) { return calm.get_input(signal_idx); }
        
        /**
         * @brief Get the input configuration for the signal
         * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
         * @return struct signal::target output configuration
         */
        static inline struct signal::target get_first_output(uint8 signal_idx) { return calm.get_first_output(signal_idx); }

        /**
         * @brief Check if the given pin is configured as an output pin
         * 
         * @param pin Pin number to check
         * @return true Pin is configured as an output
         * @return false Pin is not configured as an output
         */
        static inline bool is_output_pin(uint8 pin) { return calm.is_output_pin(pin); }
    }
}

#endif // RTE_CFG_COD_H