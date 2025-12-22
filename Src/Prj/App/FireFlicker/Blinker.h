/**
 * @file App/Blinker/Blinker.h
 *
 * @brief Configuration header file for Blinker class
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

#ifndef BLINKER_H__
#define BLINKER_H__

#include <Hal/Timer.h>
#include <Hal/Gpio.h>
#include <Hal/Random.h>
#include <Util/Array.h>

#include <Blinker_cfg.h> // include <Util/Timer.h> indirectly

template <size_t NrLamps>
class Blinker
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
    timer_array_type toggle_timers;
    /** State of output pin: either LOW or HIGH */
    int pin_value[NrLamps];

    void toggle_pin(size_t lamp_idx)
    {
        if (pin_value[lamp_idx] == HIGH)
        {
            pin_value[lamp_idx] = LOW;
            toggle_timers[lamp_idx].start(blinker_cal::get_delay_on(lamp_idx));
        }
        else
        {
            pin_value[lamp_idx] = HIGH;
            toggle_timers[lamp_idx].start(blinker_cal::get_delay_off(lamp_idx));
        }
        hal::digitalWrite(blinker_cal::get_output_pin(lamp_idx), pin_value[lamp_idx]);
    }

public:
    /** Default constructor */
    Blinker() {}
    /** Destructor has nothing to do */
    ~Blinker() {}

    /** Init function, e.g., to be called from Arduino's setup function */
    void init()
    {
        size_t i;

        for (i = 0; i < kNrLamps; i++)
        {
            hal::pinMode(blinker_cal::get_output_pin(i), OUTPUT);
            pin_value[i] = HIGH;
            toggle_timers[i].start(blinker_cal::get_delay_off(i));
        }
    }

    /// Loop function, to be called from Arduino's loop function
    void run(void)
    {
        size_t i;

        for (i = 0; i < kNrLamps; i++)
        {
            if (toggle_timers[i].timeout())
            {
                toggle_pin(i);
            }
        }
    }
};

#endif // BLINKER_H__