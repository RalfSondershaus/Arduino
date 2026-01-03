/**
 * @file FireFlicker.cpp
 * 
 * @author Ralf Sondershaus
 * 
 * @brief Implementation of fire flicker effect for multiple lamps
 * 
 * @version 0.1
 * 
 * @date 2025-11-23
 * 
 * @copyright Copyright (c) 2025 Ralf Sondershaus
 * 
 * SPDX-License-Identifier: Apache-2.0
 */

#include <Blinker.h>
#include <Blinker_cfg.h>
#include <FireFlicker.h>
#include <FireFlicker_cfg.h>
#include <Util/Array.h>
#include <Hal/Serial.h>
#include <Arduino.h>

/** @defgroup FireFlicker Configuration for FireFlicker
 *  @brief Configuration parameters for FireFlicker class
 *  @{
 */
/** Number of lamps in the fire flicker */
static constexpr size_t kNrLamps = 3;

/** Output pins for the lamps */
  static util::array<int, kNrLamps> output_pins = {2, 3, 4};
/** Number of lamps in the blinker */
static constexpr size_t kNrLamps_blinker = 1;

/** Output pins for the lamps */
static util::array<int, kNrLamps_blinker> output_pins_blinker = {13};
/** @} */

/** @defgroup Local types for FireFlicker
 *  @brief Local types used in FireFlicker implementation
 *  @{
 */
/** Fire flicker type */
using fire_flicker = FireFlicker<kNrLamps>;
/** Blinker type */
using blinker = Blinker<kNrLamps_blinker>;
/** @} */

/** @defgroup Local variables for FireFlicker
 *  @brief Local variables used in FireFlicker implementation
 *  @{
 */
/** Fire flicker instance */
fire_flicker my_fire_flicker;
/** Blinker instance */
blinker my_blinker;
/** @} */

/**
 * @brief Get the output pin number for a given lamp index
 * 
 * @param lamp_idx Lamp index (0 ... kNrLamps-1)
 * @return int Output pin number
 */
int fire_flicker_cal::get_output_pin(size_t lamp_idx)
{
    return output_pins.at(lamp_idx);
}

/**
 * @brief Get the maximum delay time for turning off the fire flicker lamp
 * 
 * @param lamp_idx Lamp index (0 ... kNrLamps-1)
 * @return util::MilliTimer::time_type Maximum delay time in milliseconds
 */
util::MilliTimer::time_type fire_flicker_cal::get_max_delay_off(size_t lamp_idx)
{
    return static_cast<util::MilliTimer::time_type>(500U);
}

/**
 * @brief Get the maximum delay time for turning on the fire flicker lamp
 * 
 * @param lamp_idx Lamp index (0 ... kNrLamps-1)
 * @return util::MilliTimer::time_type Maximum delay time in milliseconds
 */
util::MilliTimer::time_type fire_flicker_cal::get_max_delay_on(size_t lamp_idx)
{
    return static_cast<util::MilliTimer::time_type>(2000U);
}

/**
 * @brief Get the output pin number for a given lamp index in blinker
 * 
 * @param lamp_idx Lamp index (0 ... kNrLamps_blinker-1)
 * @return int Output pin number
 */
int blinker_cal::get_output_pin(size_t lamp_idx)
{
    return output_pins_blinker.at(lamp_idx);
}

/**
 * @brief Get a delay time for turning off the blinker lamp
 * 
 * @param lamp_idx Lamp index (0 ... kNrLamps_blinker-1)
 * @return util::MilliTimer::time_type Delay time in milliseconds
 */
util::MilliTimer::time_type blinker_cal::get_delay_off(size_t lamp_idx)
{
    return static_cast<util::MilliTimer::time_type>(500U);
}

/**
 * @brief Get a delay time for turning on the blinker lamp
 * 
 * @param lamp_idx Lamp index (0 ... kNrLamps_blinker-1)
 * @return util::MilliTimer::time_type Delay time in milliseconds
 */
util::MilliTimer::time_type blinker_cal::get_delay_on(size_t lamp_idx)
{
    return static_cast<util::MilliTimer::time_type>(1000U);
}

/**
 * @brief Initialize the fire flicker effect
 * 
 * This function initializes the fire flicker by setting up the output pins
 * and starting the timers for each lamp. It should be called once during the setup phase.
 */
void setup()
{
    my_fire_flicker.init();
    my_blinker.init();
}

/**
 * @brief Run the fire flicker effect
 * 
 * This function updates the fire flicker effect and should be called repeatedly in the main loop.
 * It checks the timers for each lamp and toggles their states accordingly.
 */
void loop()
{
    my_fire_flicker.run();
    my_blinker.run();
}
