/**
 * @file Hal/Win/Eeprom/Hal/EEPROM.h
 * 
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for EEPROM, abstracts Arduino's EEPROM.h to run on Windows.
 *
 * This file declares Arduino EEPROM functions such as
 * - EEPROM.write
 * - EEPROM.read
 * - EEPROM.update
 * 
 * The file Hal/EEPROM/Hal/EEPROM.h exists multiple times to support stubs. 
 * Include this file to use stub functions.
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
            constexpr int kMaxElements = 1024;
            extern uint8 elements[kMaxElements];
        }

        inline uint8 read(int idx)              { return (idx < stubs::kMaxElements) ? stubs::elements[idx] : 0xFF; }
        inline void write(int idx, uint8 val)   { if (idx < stubs::kMaxElements) { stubs::elements[idx] = val; } }
        inline void update(int idx, uint8 val)  { if (idx < stubs::kMaxElements) { stubs::elements[idx] = val; } }
    }
}

#endif // HAL_EEPROM_H