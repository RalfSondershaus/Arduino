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
  
  // -----------------------------------------------------------------------------------
  /// Init at system start
  // -----------------------------------------------------------------------------------
  void InputClassifier::init()
  {
    const cal::input_classifier_cal_type * pCal = rte::ifc_cal_input_classifier.call();
    classifiers.set_config(pCal);
  }

  // -----------------------------------------------------------------------------------
  /// Read AD values, classify and write classified values onto the RTE.
  // -----------------------------------------------------------------------------------
  void InputClassifier::cycle()
  {
    classifiers.run();

    (void)rte::write(rte::ifc_classified_values, classifiers.classified_values());
  }
}
