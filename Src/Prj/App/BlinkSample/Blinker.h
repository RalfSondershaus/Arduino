/**
 * @file Blinker.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Declares class Blinker for controlling LED blinking functionality
 *
 * This module provides a Blinker class that manages LED blinking on an Arduino board.
 * It supports PWM intensity control via pin 13 (on-board LED) and optional MOSFET control
 * via pin 2. The class implements a breathing/fading effect with configurable intensity
 * levels from 0 to 255.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef BLINKER_H_
#define BLINKER_H_

#include <Std_Types.h>
#include <Rte/Rte_Types_Ifc.h>

// --------------------------------------------------------------------------------------------
/// Blink the LED on pin 13 (LED on board).
// --------------------------------------------------------------------------------------------
class Blinker
{
protected:
  static constexpr int kLedPin = 13;
  unsigned int unIntensity;   ///< [0...255] intensity value
  bool bUp;                   ///< upwards counting (true) or downwards counting (false)

public:
  /// default constructor
  Blinker() : unIntensity(0), bUp(true)
  {}

  /// Initialization
  void init(void);

  /// Main execution function
  void run(void);
};

#endif // BLINKER_H_