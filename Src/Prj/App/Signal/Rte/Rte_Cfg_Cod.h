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
    static inline void set_cv(uint16 cv_id, uint8 val)  { calm.set_cv(cv_id, val); }
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
         * @brief Return true if the output pin order is inverse, e.g. pins 13, 12, 11, 10 instead 
         *        of 13, 14, 15, 16
         * @param signal_idx Signal index (0 ... cfg::kNrSignals-1)
         * @return true Output pin order is inverse
         * @return false Output pin order is normal
         */
        static inline bool is_output_pin_order_inverse(uint8 signal_idx) { return calm.is_output_pin_order_inverse(signal_idx); }

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
        static inline uint8 get_output_pin_step_size(uint8 signal_idx) { return calm.get_output_pin_step_size(signal_idx); }

        /**
         * @brief Check if the given pin is configured as an output pin
         * 
         * @param pin Pin number to check
         * @return true Pin is configured as an output
         * @return false Pin is not configured as an output
         */
        static inline bool is_output_pin(uint8 pin) { return calm.is_output_pin(pin); }

        /**
         * @brief Get the number of outputs for the signal.
         * 
         * @note The parameter signal_id is the signal ID as read from CV (not the signal index).
         * @note For built-in signals, the number of outputs is defined in the ROM constant array
         *       built_in_signal_outputs[].
         * @note For user-defined signals, the number of outputs is 0 by default and can be set
         *       using CVs.
         * @note If the signal ID is invalid or not used, the number of outputs is 0.
         * @note The maximum number of outputs is defined by cfg::kNrSignalTargets.
         * 
         * @param signal_id Signal id
         * @return uint8 Number of outputs for the signal
         */
        static inline uint8 get_number_of_outputs(uint8 signal_id) { return calm.get_number_of_outputs(signal_id); }
    }
}

#endif // RTE_CFG_COD_H