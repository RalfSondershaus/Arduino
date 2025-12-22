/**
 * @file App/FireFlicker/FireFlicker_cfg.h
 *
 * @brief Configuration header file for FireFlicker class
 *
 * @note This file must be included in FireFlicker.cpp only
 * @note All functions in this file are defined as inline to allow compile-time optimization
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

#ifndef FIREFLICKER_CFG_H__
#define FIREFLICKER_CFG_H__

#include <Std_Types.h>

namespace fire_flicker_cal
{
    int get_output_pin(size_t lamp_idx);
    util::MilliTimer::time_type get_max_delay_off(size_t lamp_idx);
    util::MilliTimer::time_type get_max_delay_on(size_t lamp_idx);
}

#endif // FIREFLICKER_CFG_H__