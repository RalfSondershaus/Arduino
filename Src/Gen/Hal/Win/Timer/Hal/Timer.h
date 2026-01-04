/**
 * @file Hal/Win/Timer/Timer.h
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for timers, abstracts parts of Arduino's Arduino.h for Windows
 *
 * This file calls Arduino functions such as
 * - millis
 * - micros
 * 
 * A file Timer/Timer.h exists multiple times to support stubs and architectures other
 * than Arduino.
 * 
 * This version of Timer/Timer.h supports to run the code on Windows and use default
 * C++ function to get timer values.
 * 
 * @copyright Copyright 2024 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <Std_Types.h>
#include <WinArduino.h>

namespace hal
{
    inline uint32 millis() { return ::millis(); }
    inline uint32 micros() { return ::micros(); }
}

#endif // HAL_TIMER_H