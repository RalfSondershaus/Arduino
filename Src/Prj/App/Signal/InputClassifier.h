/**
 * @file Signal/InputClassifier.h
 *
 * @author Ralf Sondershaus
 *
 * @brief The classifier interface.
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

#ifndef SIGNAL_INPUT_CLASSIFIER_H_
#define SIGNAL_INPUT_CLASSIFIER_H_

#include "Cfg_Prj.h"
#include <Rte/Rte_Type.h>
#include <Cal/CalM_Type.h>
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
    typedef util::input_classifier<cfg::kNrClassifiers, cfg::kNrClassifierClasses> input_classifier_type;
    typedef const cal::input_classifier_cal_type * cal_const_pointer;
    using classified_values_array_type = input_classifier_type::classified_values_array_type;
    
  protected:
    input_classifier_type classifiers;

  public:
    /// Construct
    InputClassifier() {}

    /// Runables
    void init();
    void cycle();
  };
} // namespace signal

#endif // SIGNAL_INPUT_CLASSIFIER_H_
