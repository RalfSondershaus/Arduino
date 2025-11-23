/**
 * @file Hal/Arduino/Random/Random.h
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for random number generation, abstracts Arduino's Arduino.h to enable stubs
 *
 * This file calls Arduino functions such as
 * - millis
 * - micros
 * 
 * A file Hal/Timer.h exists multiple times to support stubs. 
 * There is this file which shall be used when Arduino's functions shall be used.
 * Use a stub version of this file to support stubs that shall not use Arduino's functions.
 * 
 * Arduino.h declares prototypes of all relevant Arduino functions.
 * Wiring.c defines the functions init(), millis(), and micros(), beside others.
 * If you want to use init() but want to stub millis() and micros(), you need to compiler
 * wiring.c. So there will be a version of millis() and micros(). That's why
 * we introduce this hal:: abstraction layer to be able to stub millis() and micros()
 * even when wiring.c is compiled. The hal:: versions of millis() and micro() 
 * can be stubbed. In the target system, Arduino's implementation of millis() and 
 * micros() exist but are not called because the stub versions are called.
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

#ifndef HAL_RANDOM_H
#define HAL_RANDOM_H

#include <Std_Types.h>
#include <Arduino.h>

namespace hal
{
    inline uint32 random(uint32 how_big) { return ::random(how_big); }
    inline void seed_random(uint32 seed) { ::randomSeed(seed); }
}

#endif // HAL_RANDOM_H