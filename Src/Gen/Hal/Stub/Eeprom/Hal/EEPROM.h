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
 * SPDX-License-Identifier: Apache-2.0
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