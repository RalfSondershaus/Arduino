/**
 * @file Signal/InputClassifier.cpp
 *
 * @brief Read AD values, classify and write classified value onto RTE
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

#include "InputClassifier.h"
#include "Cfg_Prj.h"
#include <Rte/Rte.h>
#include <Arduino.h>

namespace signal
{
//  namespace coding
  //{
  //  /// Coding data type for AD input classifiers
  //  typedef util::coding::cfg_input_classifier<cfg::kNrClassifiers, cfg::kNrClassifierClasses, uint16> cfg_type;

  //  static constexpr uint8  kAdDebounce = 5; ///< [10 ms] Debounce time

  //  /// AD limits
  //  static constexpr uint16 kGreenLo  = (605 - 10);
  //  static constexpr uint16 kGreenHi  = (605 + 10);
  //  static constexpr uint16 kYellowLo = (399 - 10);
  //  static constexpr uint16 kYellowHi = (399 + 10);
  //  static constexpr uint16 kRedLo    = (192 - 10);
  //  static constexpr uint16 kRedHi    = (192 + 10);
  //  static constexpr uint16 kWhiteLo  = (147 - 10);
  //  static constexpr uint16 kWhiteHi  = (147 + 10);
  //  static constexpr uint16 kAdMin    = 0;
  //  static constexpr uint16 kAdMax    = 1023;

  //  static const cfg_type cfg =
  //  {
  //      { A0, A1, A2, A3, A4, A5 } // pins
  //    , { // debounce and limits
  //        {   kAdDebounce                                       // 0
  //        , { kGreenLo, kYellowLo, kRedLo, kWhiteLo, kAdMax } 
  //        , { kGreenHi, kYellowHi, kRedHi, kWhiteHi, kAdMin } 
  //        }
  //      , {   kAdDebounce                                       // 1
  //        , { kGreenLo, kYellowLo, kRedLo, kWhiteLo, kAdMax } 
  //        , { kGreenHi, kYellowHi, kRedHi, kWhiteHi, kAdMin } 
  //        }
  //      , {   kAdDebounce                                       // 2
  //        , { kGreenLo, kYellowLo, kRedLo, kWhiteLo, kAdMax } 
  //        , { kGreenHi, kYellowHi, kRedHi, kWhiteHi, kAdMin } 
  //        }
  //      , {   kAdDebounce                                       // 3
  //        , { kGreenLo, kYellowLo, kRedLo, kWhiteLo, kAdMax }
  //        , { kGreenHi, kYellowHi, kRedHi, kWhiteHi, kAdMin }
  //        }
  //      , {   kAdDebounce                                       // 4
  //        , { kGreenLo, kYellowLo, kRedLo, kWhiteLo, kAdMax }
  //        , { kGreenHi, kYellowHi, kRedHi, kWhiteHi, kAdMin }
  //        }
  //      , {   kAdDebounce                                       // 5
  //        , { kGreenLo, kYellowLo, kRedLo, kWhiteLo, kAdMax }
  //        , { kGreenHi, kYellowHi, kRedHi, kWhiteHi, kAdMin }
  //        }
  //    }
  //  };
  //} // namespace coding

  // -----------------------------------------------------------------------------------
  /// Server runable: set calibration values
  // -----------------------------------------------------------------------------------
  void InputClassifier::set_cal(const cal::input_classifier_cfg_type * p)
  {
    pCal = p;
    classifiers.set_config(pCal);
  }
  
  // -----------------------------------------------------------------------------------
  /// Init at system start
  // -----------------------------------------------------------------------------------
  void InputClassifier::init()
  {
  }

  // -----------------------------------------------------------------------------------
  /// Read AD values, classify and write classified values onto the RTE.
  // -----------------------------------------------------------------------------------
  void InputClassifier::cycle10()
  {
    classifiers.run();

    rte::ifc_classified_values.write(classifiers.classified_values());
  }
}
