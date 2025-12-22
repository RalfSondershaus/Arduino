/**
 * @file App/FireFlicker/FireFlicker.h
 *
 * @brief Configuration header file for fire_flicker class
 *
 * @copyright Copyright 2025 Ralf Sondershaus
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

#ifndef FIREFLICKER_H__
#define FIREFLICKER_H__

#include <Hal/Timer.h>
#include <Hal/Gpio.h>
#include <Hal/Random.h>
#include <Hal/Serial.h>
#include <Util/Array.h>
#include <Util/Timer.h>

#include <FireFlicker_cfg.h>

/**
 * @brief Fire flicker effect implementation for multiple lamps.
 * 
 * This class manages a fire flicker simulation for a configurable number of lamps.
 * Each lamp randomly toggles between HIGH and LOW states to create a realistic
 * flickering effect similar to a flame.
 * 
 * @tparam NrLamps Number of lamps to control in the fire flicker effect
 * 
 * The class uses random intervals to create natural-looking flicker patterns:
 * - When turning OFF (HIGH to LOW): random delay up to 500ms
 * - When turning ON (LOW to HIGH): random delay up to 2000ms
 * 
 * Usage:
 * 1. Create a FireFlickerSetting object with pin configurations
 * 2. Instantiate FireFlicker with the settings
 * 3. Call init() once during setup
 * 4. Call run() repeatedly in the main loop
 * 
 * @note The default constructor is protected and should not be used directly.
 *       Always use the constructor that accepts a FireFlickerSetting reference.
 */
template <size_t NrLamps>
class FireFlicker
{
    public:
    /** Number of lamps in the fire flicker */
    static constexpr size_t kNrLamps = NrLamps;

    protected:
    /** Type used for time stamps */
    using timer = util::MilliTimer;
    /** Array type for time stamps */
    using timer_array_type = util::array<timer, NrLamps>;
    /** Array of next toggle times */
    timer_array_type timers;
    /** State of output pin: either LOW or HIGH */
    int pin_value[NrLamps];

public:
    /** Default constructor */
    FireFlicker() {}
    /** Destructor has nothing to do */
    ~FireFlicker() {}
    /** Init function, e.g., to be called from Arduino's setup function */
    void init()
    {
        size_t i;

        hal::seed_random(hal::millis());

        for (i = 0; i < kNrLamps; i++)
        {
            hal::pinMode(fire_flicker_cal::get_output_pin(i), OUTPUT);
            pin_value[i] = HIGH;
            timers[i].start(hal::random(fire_flicker_cal::get_max_delay_off(i)));
        }
    }

    /// Loop function, to be called from Arduino's loop function
    void run(void)
    {
        size_t i;

        for (i = 0; i < kNrLamps; i++)
        {
            if (timers[i].timeout())
            {
                if (pin_value[i] == HIGH)
                {
                    pin_value[i] = LOW;
                    timers[i].start(hal::random(500));
                }
                else
                {
                    pin_value[i] = HIGH;
                    timers[i].start(hal::random(fire_flicker_cal::get_max_delay_on(i)));
                }
                hal::digitalWrite(fire_flicker_cal::get_output_pin(i), pin_value[i]);
            }
        }
    }
};

#endif // FIREFLICKER_H__