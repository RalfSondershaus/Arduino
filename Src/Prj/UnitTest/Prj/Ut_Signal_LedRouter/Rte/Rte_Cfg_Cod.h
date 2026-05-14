/**
 * @file Rte_Cfg_Cod.h
 *
 * @author Ralf Sondershaus
 *
 * @brief RTE for calibration data of LedRouter unit test.
 *
 * Provides access to calibration data and signal aspects for the LedRouter unit test.
 * The file is included by the LedRouter unit test and provides access to the data that CalM 
 * typically manages.
 * 
 * @note Just provides the necessary data and functions for the unit test, so it is not a complete 
 * configuration file.
 * 
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef RTE_CFG_COD_H
#define RTE_CFG_COD_H

#include <Std_Types.h>
#include <Prj_Types.h>

namespace rte
{
    /**
     * @brief Namespace for signal-related RTE functions
     * 
     * @note Use namespace name 'sig' to avoid confusion with namespace 'signal'
     */
    namespace sig
    {
        /**
         * @brief Check if the given pin is configured as an output pin
         * 
         * @param pin Pin number to check (not used in this unit test, so it can be ignored)
         * @return true Pin is configured as an output
         * @return false Pin is not configured as an output
         */
        static inline bool is_output_pin(uint8 pin) { (void)pin; return true; }
    }
}

#endif // RTE_CFG_COD_H