/**
 * @file BlinkSample2.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Declares class Blinker
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