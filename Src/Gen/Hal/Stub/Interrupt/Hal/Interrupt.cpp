/**
 * @file Hal/Stub/Gpio/Gpio.cpp
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for GPIOs, defines stubs for parts of Arduino's Arduino.h
 *
 * This file defines stub functions for Arduino functions such as
 * - pinMode
 * - digitalWrite
 * - digitalRead
 * - analogRead
 * - analogWrite
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

#include <Hal/Interrupt.h>

namespace hal
{
    namespace stubs
    {
        uint8 isr_nr;
        func_pointer func;
        int isr_mode;
    }
}
