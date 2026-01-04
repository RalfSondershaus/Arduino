/**
 * @file App/FireFlicker/Blinker_cfg.h
 *
 * @brief Configuration header file for Blinker class
 *
 * @note All functions in this file are defined as inline to allow compile-time optimization
 *
 * @copyright Copyright 2025 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLINKER_CFG_H__
#define BLINKER_CFG_H__

#include <Std_Types.h>
#include <Util/Timer.h>

namespace blinker_cal
{
    int get_output_pin(size_t lamp_idx);
    util::MilliTimer::time_type get_delay_off(size_t lamp_idx);
    util::MilliTimer::time_type get_delay_on(size_t lamp_idx);
}

#endif // BLINKER_CFG_H__