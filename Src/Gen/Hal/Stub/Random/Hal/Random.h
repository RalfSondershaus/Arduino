/**
 * @file Hal/Stub/Random/Random.h
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
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HAL_TIMER_H
#define HAL_TIMER_H

#include <Std_Types.h>

namespace hal
{
    namespace stubs
    {
        extern uint32 random;
    }

    inline uint32 random(uint32 how_big) { return stubs::random; }
    inline void seed_random(uint32 seed) { (void)seed; }
}

#endif // HAL_TIMER_H