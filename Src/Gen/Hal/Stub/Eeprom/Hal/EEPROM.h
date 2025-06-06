/**
 * @file Hal/Stub/Eeprom/Hal/EEPROM.h
 * 
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for EEPROM, abstracts Arduino's EEPROM.h to enable stubs
 *
 * This file declares Arduino EEPROM functions such as
 * - EEPROM.write
 * - EEPROM.read
 * - EEPROM.update
 * 
 * The file Hal/EEPROM/Hal/EEPROM.h exists multiple times to support stubs. 
 * Include this file to use stub functions.
 * 
 * EEPROM functionality is defined in Arduino's EEPROM library.
 * 
 * @copyright Copyright 2024 Ralf Sondershaus
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

#ifndef HAL_EEPROM_H
#define HAL_EEPROM_H

#include <Std_Types.h>

namespace hal
{
    namespace eeprom
    {
        static constexpr uint8 kInitial = 0xFF;
        
        namespace stubs
        {
            // NANO: 1 KB, MEGA: 4 KB
            // Shall not exceed number of used EEPROM bytes.
            // For example, unit test for Signal uses cal::eeprom::eSizeOfData number of bytes
            constexpr int kMaxElements = 256; 
            extern uint8 elements[kMaxElements];
        }

        inline uint8 read(int idx)              { return (idx < stubs::kMaxElements) ? stubs::elements[idx] : 0xFF; }
        inline void write(int idx, uint8 val)   { if (idx < stubs::kMaxElements) { stubs::elements[idx] = val; } }
        inline void update(int idx, uint8 val)  { if (idx < stubs::kMaxElements) { stubs::elements[idx] = val; } }
    }
}

#endif // HAL_EEPROM_H