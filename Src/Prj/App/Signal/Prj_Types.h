/**
 * @file Gen/Util/Gen_Types.h
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

    /** 
     * @brief Input source: classified AD values, commands received via busses (such as DCC), etc 
     */
    struct input
    {
        static constexpr uint8 kDcc = cal::signal::kDcc; /**< DCC input type */
        static constexpr uint8 kAdc = cal::signal::kAdc; /**< ADC input type */
        static constexpr uint8 kDig = cal::signal::kDig; /**< Digital input type */

        uint8 type; ///< Type of command source [kDcc, kAdc, kDig]
        uint8 idx;  ///< Index of the input element on RTE
    };

    /**
     * @brief Define target outputs such as onboard or external and which output pin
     */
    struct target
    {
        static constexpr uint8 kOnboard = cal::signal::kOnboard;   ///< Onboard output type
        static constexpr uint8 kExternal = cal::signal::kExternal; ///< External output type

        /** Constructor from CV value*/
        target(uint8 v)
        {
            *this = v;
        }

        /** Convert to CV value*/
        explicit operator uint8() const 
        { 
            return util::bits::lshift(type, cal::signal::bitshift::kFirstOutputType) |
                   util::bits::lshift(pin, cal::signal::bitshift::kFirstOutputPin); 
        }

        /** Assignment from CV value*/
        struct target operator=(const uint8 v)
        {
            pin = util::bits::masked_shift(v, cal::signal::bitmask::kFirstOutputPin, cal::signal::bitshift::kFirstOutputPin);
            type = util::bits::masked_shift(v, cal::signal::bitmask::kFirstOutputType, cal::signal::bitshift::kFirstOutputType);
            return *this;
        }

        uint8 pin : (cal::signal::bitshift::kFirstOutputType);         ///< output pin number
        uint8 type : (8U - cal::signal::bitshift::kFirstOutputType); ///< type of target (kOnboard, kExternal)
    };

}

#endif // UTIL_GEN_TYPES_H_
