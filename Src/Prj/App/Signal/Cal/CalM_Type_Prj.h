/**
 * @file Cal/CalM_Type_Prj.h
 *
 * @brief Defines project specific calibration types.
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

#ifndef CALM_TYPE_PRJ_H_
#define CALM_TYPE_PRJ_H_

#include <Cfg_Prj.h>
#include <Util/Array.h>
#include <Util/Classifier.h>

namespace cal
{
  /// Define inputs: classified AD values, commands received via busses (such as DCC), etc
  typedef struct
  {
    enum
    {
      kNone = 0,
      kClassified = 1
    };
    uint8 type : 2; ///< [kNone, kClassified] type of command sources, more in future
    uint8 idx : 6; ///< Index of the input element on RTE
  } input_type;

  /// Define target outputs such as onboard or external and which output pin
  typedef struct
  {
    enum
    {
      kNone = 0,
      kOnboard = 1,
      kExternal = 2
    };

    uint8 type : 2; ///< type of target such as kNone, kOnboard, kExternal
    uint8 idx : 6; ///< output pin number
  } target_type;

  /// Define signal target intensities
  typedef struct
  {
    uint8 aspect; ///< A bit per output, max. 8 outputs: 0 = 0%, 1 = 100%, LSB = 1st output, MSB = 8th output
    uint8 blink;  ///< A bit per output, max. 8 outputs: 0 = no blinking, 1 = blinking
  } aspect_type;

  /// Coding data type for a signal
  typedef struct
  {
    input_type input;
    util::array<aspect_type, cfg::kNrSignalAspects> aspects;
    util::array<target_type, cfg::kNrSignalTargets> targets;
    uint8 changeOverTime; ///< [10 ms] dim time if aspect changes
    uint8 blinkChangeOverTime; ///< [10 ms] dim time for blinking effects
  } signal_type;

  /// Coding data for each signal
  typedef util::array<signal_type, cfg::kNrSignals> signal_array;

  /// Calibration data for input classifiers is taken over from util::input_classifier
  typedef util::input_classifier<cfg::kNrClassifiers, cfg::kNrClassifierClasses> input_classifier_type;
  using input_classifier_cfg_type = input_classifier_type::input_classifier_cfg_type;

} // namespace cal

#endif // CALM_TYPE_PRJ_H_
