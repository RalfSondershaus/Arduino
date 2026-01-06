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

#include <Hal/Gpio.h>
#include <Util/Array.h>

namespace hal
{
    namespace stubs
    {
        util::array<uint8, kNrPins> pinMode;
        util::array<uint8, kNrPins> digitalWrite;
        util::array<uint8, kNrPins> digitalRead;
        util::array<int, kNrPins> analogWrite;
        util::array<int, kNrPins> analogRead;
        uint8_t analogReference;
    }

    void init_gpio()
    {
        stubs::pinMode.fill(0);
        stubs::digitalWrite.fill(0);
        stubs::digitalRead.fill(0);
        stubs::analogWrite.fill(0);
        stubs::analogRead.fill(0);
    }
}
