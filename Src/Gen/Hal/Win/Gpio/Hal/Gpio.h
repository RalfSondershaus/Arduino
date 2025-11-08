/**
 * @file Hal/Win/Gpio/Gpio.h
 * @author Ralf Sondershaus
 *
 * @brief HAL layer for GPIOs, abstracts Arduino's Arduino.h to enable stubs
 *
 * This file calls Arduino functions such as
 * - pinMode
 * - digitalWrite
 * - digitalRead
 * - analogRead
 * - analogWrite
 * 
 * The file Hal/Gpio/Gpio.h exists multiple times to support stubs. 
 * Include this file to use Arduino's functions.
 * Include a stub version of this file to support stubs that shall not use Arduino's functions.
 * 
 * Arduino.h declares prototypes of all relevant Arduino functions.
 * Wiring_analog.c and wiring_digital.c define the functions digitalWrite(), analogRead(), etc.
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

#ifndef HAL_GPIO_H
#define HAL_GPIO_H

#include <Util/Array.h>
#include <WinArduino.h>

namespace hal
{
    static constexpr uint8_t kNrDigitalPins = 70;

    inline void pinMode        (uint8_t pin, uint8_t mode)     { ::pinMode(pin, mode); }
    inline void digitalWrite   (uint8_t pin, uint8_t value)    { ::digitalWrite(pin, value); }
    inline int  digitalRead    (uint8_t pin)                   { return ::digitalRead(pin); }
    inline int  analogRead     (uint8_t pin)                   { return ::analogRead(pin); }
    inline void analogReference(uint8_t mode)                  { ::analogReference(mode); }
    inline void analogWrite    (uint8_t pin, int value)        { ::analogWrite(pin, value); }

    /**
     * @brief Configuration structure for GPIO pins.
     * 
     * @note The size of pin_array is kNrDigitalPins.
     */
    struct GpioConfig
    {
        using pin_array = util::array<uint8_t, kNrDigitalPins>;
        /**
         * @brief Array of pin modes for each GPIO pin.
         * 
         * Possible values for each pin mode are:
         * - INPUT
         * - OUTPUT
         * - INPUT_PULLUP
         */
        pin_array pin_modes;
    };

    /**
     * @brief Setup GPIO pins according to the provided configuration.
     * @param config The GPIO configuration.
     * 
     * This function sets the pin modes for all GPIO pins based on the provided configuration.
     * Each pin's mode is set using the Arduino pinMode function.
     */
    inline void setup_gpio_pins(const struct GpioConfig& config)
    {
        for (size_t pin = 0; pin < kNrDigitalPins; pin++)
        {
            pinMode(pin, config.pin_modes[pin]);
        }
    }
}

#endif // HAL_GPIO_H