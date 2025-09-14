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
// #include <Util/Logger.h>

namespace signal
{
  // static util::logger log;
  // -----------------------------------------------------------------------------------
  /// Init at system start
  // -----------------------------------------------------------------------------------
  void InputClassifier::init()
  {
    const cal::classifier_array_cal_type * pCal = rte::ifc_cal_input_classifier::call();
    classifiers.set_config(pCal);
    // log.start(1000);
  }

  // -----------------------------------------------------------------------------------
  /// Read AD values, classify and write classified values onto the RTE.
  // -----------------------------------------------------------------------------------
  void InputClassifier::cycle()
  {
    classifiers.run();

    rte::ifc_classified_values::write(classifiers.get_classified_values());
    rte::ifc_ad_values::write(classifiers.get_adc_values());
    // log.begin("AD Values ");
    // auto aADValues = classifiers.ad_values();
    // for (auto it = aADValues.begin(); it != aADValues.end(); it++)
    // {
    //   log << " " << *it;
    // }
    // log << " - ";
    // auto aCLValues = classifiers.classified_values();
    // for (auto it = aCLValues.begin(); it != aCLValues.end(); it++)
    // {
    //   log << " " << static_cast<int>(*it);
    // }
    // log.end();
  }

  /**
   * @brief 
   * 
   */
  rte::ret_type InputClassifier::set_config(uint8 classifier_pos)
  {
    const cal::classifier_array_cal_type * pCal = rte::ifc_cal_input_classifier::call();
    classifiers.set_config(pCal, classifier_pos);
    return rte::ret_type::OK;
  }
}
