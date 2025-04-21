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

#include <Std_Types.h>

#define NOT_AN_INTERRUPT -1

#define CHANGE 1
#define FALLING 2
#define RISING 3

namespace hal
{
    using func_pointer = void(*)(void);
    namespace stubs
    {
        
        extern uint8 isr_nr;
        extern func_pointer func;
        extern int isr_mode; // CHANGE, FALLING, RISING
    }

    // This is the MEGA variant
    #define digitalPinToInterrupt(p) ((p) == 2 ? 0 : ((p) == 3 ? 1 : ((p) >= 18 && (p) <= 21 ? 23 - (p) : NOT_AN_INTERRUPT)))

    inline void attachInterrupt(uint8_t isr_nr, func_pointer func, int mode) 
    {
        stubs::isr_nr = isr_nr;
        stubs::func = func;
        stubs::isr_mode = mode;
    }
    inline void detachInterrupt(uint8_t isr_nr)
    {
        stubs::isr_nr = isr_nr;
    }
}

#endif // HAL_GPIO_H