/**
 * @file Signal/InputClassifier.cpp
 *
 * @brief Read AD values, classify and write classified value onto RTE
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#include "InputClassifier.h"
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
}
