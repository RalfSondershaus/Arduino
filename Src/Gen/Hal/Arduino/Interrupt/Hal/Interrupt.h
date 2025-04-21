/**
 * @file Hal/Stub/Interrupt/Interrupt.h
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for Interrupt functions, abstracts Arduino's Arduino.h to enable stubs
 *
 * The file Hal/Interrupt/Interrupt.h exists multiple times to support stubs. 
 * Include this file to use Arduino's functions.
 * Include a stub version of this file to support stubs that shall not use Arduino's functions.
 * 
 * Arduino.h declares prototypes of all relevant Arduino functions.
 * Wiring_analog.c and wiring_digital.c define the functions digitalWrite(), analogRead(), etc.
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

#ifndef HAL_INTERRUPT_H
#define HAL_INTERRUPT_H

#include <Arduino.h>

namespace hal
{
    using func_pointer = void(*)(void);

    inline void attachInterrupt(uint8_t isr_nr, func_pointer func, int mode) { ::attachInterrupt(isr_nr, func, mode); }
    inline void detachInterrupt(uint8_t isr_nr) { ::detachInterrupt(isr_nr); }
}

#endif // HAL_GPIO_H