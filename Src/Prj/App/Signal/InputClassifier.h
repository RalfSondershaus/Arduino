/**
 * @file Signal/InputClassifier.h
 *
 * @author Ralf Sondershaus
 *
 * @brief The classifier interface.
 *
 * @copyright Copyright 2022 Ralf Sondershaus
 *
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef SIGNAL_INPUT_CLASSIFIER_H_
#define SIGNAL_INPUT_CLASSIFIER_H_

#include "Cfg_Prj.h"
#include <Util/Array.h>
#include <Util/Classifier.h>

namespace signal
{
  // -----------------------------------------------------------------------------------
  /// Read AD values, classify and write classified values onto the RTE.
  // -----------------------------------------------------------------------------------
  class InputClassifier
  {
  public:
    using classifier_array_type = util::classifier_array<cfg::kNrClassifiers, cfg::kNrClassifierClasses>;
    using classified_values_array_type = classifier_array_type::classified_values_array_type;
    
  protected:
    classifier_array_type classifiers;

  public:
    /// Construct
    InputClassifier() {}

    // Runables
    /// @brief Init runable
    /// @note Average run time 10 usec @ATmega2560 @16 MHz
    void init();
    /// @brief Cyclic runable
    /// @note Average run time 85 usec @ATmega2560 @16 MHz
    void cycle();
  };
} // namespace signal

#endif // SIGNAL_INPUT_CLASSIFIER_H_
