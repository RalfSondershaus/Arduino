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
 * SPDX-License-Identifier: Apache-2.0
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