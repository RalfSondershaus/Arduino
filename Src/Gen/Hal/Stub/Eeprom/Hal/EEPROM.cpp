/**
 * @file Hal/Stub/Eeprom/Hal/EERPOM.cpp
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for EEPROMM, defines stubs for parts of Arduino's EERPOM.h
 *
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Hal/EEPROM.h>

namespace hal
{
    namespace eeprom
    {
        namespace stubs
        {
            uint8 elements[kMaxElements] = { 0xFF };
        }
    }
}
