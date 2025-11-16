/**
 * @file Prj/App/Signal/Prj_Types.h
 *
 * @brief Defines generic types that can be useful in various modules
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

#ifndef UTIL_GEN_TYPES_H_
#define UTIL_GEN_TYPES_H_

#include <Std_Types.h>
#include <Platform_Limits.h>
#include <Cal/CalM_Types.h>
#include <Util/bitset.h>

namespace signal
{
    constexpr uint8 kInvalidCmd = platform::numeric_limits<uint8>::max_();

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
     * @brief Input source: classified AD values, commands received via busses (such as DCC), etc.
     * 
     * Defines the type of input source and the index of the input element on RTE.
     * Is used by Signal to query the input source configuration from InputCommand.
     * Is NOT used to query calibration data from CalM.
     */
    struct input_cmd
    {
        static constexpr uint8 kDcc = cal::constants::kDcc; /**< DCC input type */
        static constexpr uint8 kAdc = cal::constants::kAdc; /**< ADC input type */
        static constexpr uint8 kDig = cal::constants::kDig; /**< Digital input type */

        uint8 type; ///< Type of command source [kDcc, kAdc, kDig]
        uint8 idx;  ///< Index of the input element on RTE
    };

    /** 
     * @brief Input source configuration from calibration data
     * 
     * CV value defining the input type and pin for a signal.
     */
    struct input_cal
    {
        static constexpr uint8 kDcc = cal::constants::kDcc; /**< DCC input type */
        static constexpr uint8 kAdc = cal::constants::kAdc; /**< ADC input type */
        static constexpr uint8 kDig = cal::constants::kDig; /**< Digital input type */

        uint8 type; ///< Type of command source [kDcc, kAdc, kDig]
        uint8 pin;  ///< Index of the input element on RTE
    };

    /**
     * @brief Define target outputs such as onboard or external and which output pin
     */
    struct target
    {
        static constexpr uint8 kOnboard = cal::constants::kOnboard;   ///< Onboard output type
        static constexpr uint8 kExternal = cal::constants::kExternal; ///< External output type

        /** Constructor from CV value*/
        target(uint8 v)
        {
            *this = v;
        }

        /** Convert to CV value*/
        explicit operator uint8() const 
        { 
            return util::bits::lshift(type, cal::constants::bitshift::kFirstOutputType) |
                   util::bits::lshift(pin, cal::constants::bitshift::kFirstOutputPin); 
        }

        /** Assignment from CV value*/
        struct target operator=(const uint8 v)
        {
            pin = util::bits::masked_shift(v, cal::constants::bitmask::kFirstOutputPin, cal::constants::bitshift::kFirstOutputPin);
            type = util::bits::masked_shift(v, cal::constants::bitmask::kFirstOutputType, cal::constants::bitshift::kFirstOutputType);
            return *this;
        }

        uint8 pin : (cal::constants::bitshift::kFirstOutputType);       ///< output pin number
        uint8 type : (8U - cal::constants::bitshift::kFirstOutputType); ///< type of target (kOnboard, kExternal)
    };

}

#endif // UTIL_GEN_TYPES_H_
