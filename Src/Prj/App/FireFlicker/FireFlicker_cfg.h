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
 * SPDX-License-Identifier: Apache-2.0
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