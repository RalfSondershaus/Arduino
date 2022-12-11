/**
 * @file Cfg_Prj.h
 *
 * @author Ralf Sondershaus
 *
 * @brief 
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

#ifndef CFG_PRJ_H_
#define CFG_PRJ_H_

#include <Std_Types.h>

namespace cfg
{
  constexpr int kNrClassifiers = 6; ///< Number of (AD value) classifiers
  constexpr int kNrClassifierClasses = 5; ///< Number of classifier classes

  constexpr uint8 kNrSignals = 6; ///< Maximal number of signals
  constexpr uint8 kNrSignalAspects = 5; ///< Maximal number of signal aspects
  constexpr uint8 kNrSignalTargets = 5; ///< Maximal number of signal LED strings

  constexpr int kNrOnboardTargets = 54; ///< We support up to 54 onboard LED targets (some of them with PWM))
  constexpr int kNrExternalTargets = 32; ///< We support up to 32 external outputs (such as 4 shift registers with 8 bit each)
  constexpr int kNrTargets = kNrOnboardTargets + kNrExternalTargets;
} // namespace cfg

#endif // CFG_PRJ_H_
