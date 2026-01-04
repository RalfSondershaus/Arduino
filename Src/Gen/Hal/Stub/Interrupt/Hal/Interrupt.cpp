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
 * SPDX-License-Identifier: Apache-2.0
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
