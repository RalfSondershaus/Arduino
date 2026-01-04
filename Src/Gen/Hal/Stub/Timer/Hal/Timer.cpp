/**
 * @file Hal/Stub/Timer/Timer.c
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for timers, provides stubs for parts of Arduino's Arduino.h
 *
 * This file provides stubs for Arduino functions such as
 * - millis
 * - micros
 * 
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */


#include <Hal/Timer.h>

namespace hal
{
    namespace stubs
    {
        uint32 micros;
        uint32 millis;
    }
}
