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

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <WinArduino.h>

namespace hal
{
    unsigned long millis() { return ::millis(); }
    unsigned long micros() { return ::micros(); }
}

#endif // HAL_TIMER_H