/**
 * @file BlinkSample2.h
 *
 * @author Ralf Sondershaus
 *
 * @brief Declares class Blinker
 *
 * @copyright Copyright 2022 Ralf Sondershaus
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

#ifndef BLINKER_H_
#define BLINKER_H_

#include <Std_Types.h>
#include <Rte/Rte_Type_Ifc.h>

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